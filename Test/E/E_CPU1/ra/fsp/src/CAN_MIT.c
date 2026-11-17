//
// Created by Qw011 on 2026/6/20.
//
//
// Created by Qw011 on 2026/6/20.
//
#include "CAN_MIT.h"
/************************* CAN 回调函数 *************************/
const canfd_afl_entry_t p_canfd0_afl[CANFD_CFG_AFL_CH0_RULE_NUM] = {0};

/* 接收入队（实现在文件末尾） */
static void canfd_rx_push(const can_frame_t * frame);

void canfd0_callback(can_callback_args_t * p_args)
{
    if (NULL == p_args) return;
    switch (p_args->event)
    {
        case CAN_EVENT_RX_COMPLETE:
            canfd_rx_push(&p_args->frame);     /* 新增：中断里只把帧搬进队列 */
            break;
        case CAN_EVENT_TX_COMPLETE:
        case CAN_EVENT_ERR_WARNING:
        case CAN_EVENT_ERR_PASSIVE:
        case CAN_EVENT_ERR_BUS_OFF:
        case CAN_EVENT_ERR_CHANNEL:
        case CAN_EVENT_ERR_GLOBAL:
            break;
        default:
            break;
    }
}

/************************* CAN 发送函数 *************************/
fsp_err_t canfd_send_test_data(can_ctrl_t * p_ctrl, uint32_t id, uint8_t * p_data)
{
    // 防呆拦截：如果指针为空，直接退出，保护系统不卡死
    if (p_ctrl == NULL || p_data == NULL) {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    can_frame_t tx_frame = {0};
    tx_frame.id             = id;
    tx_frame.id_mode        = CAN_ID_MODE_STANDARD;
    tx_frame.type           = CAN_FRAME_TYPE_DATA;
    tx_frame.data_length_code = 8;
    tx_frame.options        = 0;

    // 复制 8 字节数据
    for (uint8_t i = 0; i < 8; i++)
    {
        tx_frame.data[i] = p_data[i];
    }


    return R_CANFD_Write(p_ctrl, CANFD_TX_BUFFER_FIFO_COMMON_0, &tx_frame);
}


/* ============================================================================
 *                          CAN 接收（新增部分）
 *
 *  电机：StackForce 6010，依据《6010 关节电机用户文档（双编）V1.2》3.1 节
 *  反馈帧（心跳包）ID = 0x780 + 电机ID，8 字节，电机使能后才会回传：
 *
 *    Byte0 : ID(低 4 位) | ERR(高 4 位)
 *    Byte1 : POS[15:8]      Byte2 : POS[7:0]
 *    Byte3 : VEL[11:4]      Byte4 : VEL[3:0] T[11:8]
 *    Byte5 : T[7:0]         Byte6 : TMos      Byte7 : TMotor
 *
 *  换算：物理量 = raw * (max - min) / (2^bits - 1) + min
 *    位置 ±12.56 rad(16bit)，速度 ±45 rad/s(12bit)，力矩 ±18 N·m(12bit)，温度 0~100℃(8bit)
 *
 *  手册示例：781 00 7F A6 7F F8 16 4B 44
 *    POS=0x7FA6(32678) → -0.034rad，VEL=0x7FF(2047) → 0rad/s，T=0x816(2070) → 0.198Nm
 * ============================================================================ */

static canfd_rx_frame_t  s_rx_queue[CANFD_RX_QUEUE_SIZE];
static volatile uint32_t s_rx_head = 0U;        /* 中断写 */
static volatile uint32_t s_rx_tail = 0U;        /* 主循环读 */

volatile canfd_rx_stat_t g_canfd_rx_stat;
canfd_motor_fb_t         g_motor_fb[CANFD_MOTOR_NUM];
canfd_rx_frame_t         g_canfd_last_unknown;
volatile uint32_t        g_canfd_unknown_cnt = 0U;

/* 没有时间基准时返回 0；在别处实现同名函数即可覆盖 */
__attribute__((weak)) uint32_t canfd_get_tick_ms(void)
{
    return 0U;
}

static void canfd_rx_push(const can_frame_t * frame)
{
    uint32_t head = s_rx_head;
    uint32_t next = (head + 1U) & (CANFD_RX_QUEUE_SIZE - 1U);

    g_canfd_rx_stat.rx_total++;

    if (next == s_rx_tail)              /* 队列满：丢新帧，不覆盖未读数据 */
    {
        g_canfd_rx_stat.rx_dropped++;
        return;
    }

    canfd_rx_frame_t * slot = &s_rx_queue[head];

    slot->id  = frame->id;
    slot->dlc = (frame->data_length_code > CANFD_RX_DATA_MAX)
                    ? (uint8_t)CANFD_RX_DATA_MAX
                    : frame->data_length_code;

    for (uint8_t i = 0; i < slot->dlc; i++)
    {
        slot->data[i] = frame->data[i];
    }

    __DMB();                            /* 数据写完再更新 head */
    s_rx_head = next;
}

static float uint_to_float(uint32_t x_int, float x_min, float x_max, uint8_t bits)
{
    float span = x_max - x_min;
    float max_val = (float)((1UL << bits) - 1UL);

    return ((float)x_int) * span / max_val + x_min;
}

/* 解析心跳包（手册 3.1） */
static void canfd_parse_feedback(uint8_t idx, const uint8_t * d)
{
    uint32_t pos_raw = ((uint32_t)d[1] << 8) | (uint32_t)d[2];
    uint32_t vel_raw = ((uint32_t)d[3] << 4) | ((uint32_t)d[4] >> 4);
    uint32_t tor_raw = (((uint32_t)d[4] & 0x0FU) << 8) | (uint32_t)d[5];

    canfd_motor_fb_t * fb = &g_motor_fb[idx];

    fb->position = uint_to_float(pos_raw, MOTOR_POS_MIN, MOTOR_POS_MAX, 16);
    fb->velocity = uint_to_float(vel_raw, MOTOR_VEL_MIN, MOTOR_VEL_MAX, 12);
    fb->torque   = uint_to_float(tor_raw, MOTOR_TOR_MIN, MOTOR_TOR_MAX, 12);
    fb->t_mos    = uint_to_float(d[6], MOTOR_TEMP_MIN, MOTOR_TEMP_MAX, 8);
    fb->t_rotor  = uint_to_float(d[7], MOTOR_TEMP_MIN, MOTOR_TEMP_MAX, 8);

    fb->err = (uint8_t)((d[0] >> 4) & 0x0FU);
    fb->id  = (uint8_t)(d[0] & 0x0FU);

    for (uint8_t i = 0; i < 8U; i++)
    {
        fb->raw[i] = d[i];
    }

    fb->stamp_ms = canfd_get_tick_ms();
    fb->online   = true;
}

void canfd_rx_process(void)
{
    uint32_t now = canfd_get_tick_ms();

    while (s_rx_tail != s_rx_head)
    {
        canfd_rx_frame_t * f = &s_rx_queue[s_rx_tail];

        /* 0x781 ~ 0x784 → 对应 0 ~ 3 号电机 */
        if ((f->id > CANFD_ID_FEEDBACK_BASE) &&
            (f->id <= CANFD_ID_FEEDBACK_BASE + CANFD_MOTOR_NUM) &&
            (f->dlc >= 8U))
        {
            canfd_parse_feedback((uint8_t)(f->id - CANFD_ID_FEEDBACK_BASE - 1U), f->data);
        }
        else if (f->dlc > 0U)
        {
            /* 不认识的 ID：留一帧做调试 */
            g_canfd_last_unknown = *f;
            g_canfd_unknown_cnt++;
        }

        __DMB();
        s_rx_tail = (s_rx_tail + 1U) & (CANFD_RX_QUEUE_SIZE - 1U);
    }

    if (0U != now)                      /* 没有时间基准就跳过掉线判断 */
    {
        for (uint8_t i = 0U; i < CANFD_MOTOR_NUM; i++)
        {
            if (0U != g_motor_fb[i].stamp_ms)
            {
                g_motor_fb[i].online =
                    ((now - g_motor_fb[i].stamp_ms) <= (uint32_t)CANFD_MOTOR_TIMEOUT_MS);
            }
        }
    }
}

bool canfd_get_motor_fb(uint8_t motor_idx, canfd_motor_fb_t * out)
{
    if ((NULL == out) || (motor_idx >= CANFD_MOTOR_NUM))
    {
        return false;
    }

    *out = g_motor_fb[motor_idx];
    return out->online;
}

void canfd_rx_flush(void)
{
    s_rx_tail = s_rx_head;
    g_canfd_rx_stat.rx_dropped = 0U;
    g_canfd_unknown_cnt        = 0U;
}
