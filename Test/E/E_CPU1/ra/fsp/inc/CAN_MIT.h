//
// Created by Qw011 on 2026/6/20.
//

#ifndef E_CAN_MIT_H
#define E_CAN_MIT_H

/* ==================== CAN 接收（新增，其它部分未动） ==================== */
#include <stdbool.h>
#include <stdint.h>

/* StackForce 6010：反馈帧 ID = 0x780 + 电机ID，8 字节，使能后才会回传 */
#define CANFD_RX_QUEUE_SIZE     (16U)       /* 队列深度，必须是 2 的幂 */
#define CANFD_RX_DATA_MAX       (64U)       /* CAN FD 一帧最多 64 字节 */

#define CANFD_ID_FEEDBACK_BASE  (0x780U)    /* 心跳包 ID 基址 */
#define CANFD_MOTOR_NUM         (4U)        /* 电机数：ID 0x01 ~ 0x04 */
#define CANFD_MOTOR_TIMEOUT_MS  (100U)      /* 多久没收到就认为掉线 */

/* 手册 3.1 的出厂量程，串口改过的话这里要同步 */
#define MOTOR_POS_MIN           (-12.56f)   /* rad */
#define MOTOR_POS_MAX           ( 12.56f)
#define MOTOR_VEL_MIN           (-45.0f)    /* rad/s */
#define MOTOR_VEL_MAX           ( 45.0f)
#define MOTOR_TOR_MIN           (-18.0f)    /* N·m */
#define MOTOR_TOR_MAX           ( 18.0f)
#define MOTOR_TEMP_MIN          (  0.0f)    /* ℃ */
#define MOTOR_TEMP_MAX          (100.0f)

/* 一帧原始数据 */
typedef struct
{
    uint32_t id;
    uint8_t  dlc;                       /* 有效字节数 */
    uint8_t  data[CANFD_RX_DATA_MAX];
} canfd_rx_frame_t;

/* 心跳包解出来的物理量 */
typedef struct
{
    float    position;      /* rad */
    float    velocity;      /* rad/s */
    float    torque;        /* N·m */
    float    t_mos;         /* MOS 温度 ℃ */
    float    t_rotor;       /* 线圈温度 ℃ */
    uint8_t  err;           /* Byte0 高 4 位 */
    uint8_t  id;            /* Byte0 低 4 位 */
    uint32_t stamp_ms;
    bool     online;
    uint8_t  raw[8];        /* 原始字节，调试用 */
} canfd_motor_fb_t;

typedef struct
{
    volatile uint32_t rx_total;
    volatile uint32_t rx_dropped;
    volatile uint32_t err_bus_off;
    volatile uint32_t err_warning;
    volatile uint32_t err_passive;
    volatile uint32_t err_global;
} canfd_rx_stat_t;

extern volatile canfd_rx_stat_t g_canfd_rx_stat;
extern canfd_motor_fb_t         g_motor_fb[CANFD_MOTOR_NUM];
extern canfd_rx_frame_t         g_canfd_last_unknown;
extern volatile uint32_t        g_canfd_unknown_cnt;

/* 主循环里调用一次即可（帧在中断里入队，这里出队解析） */
void canfd_rx_process(void);

/* 取某台电机的反馈快照；返回 false 表示编号非法或掉线。idx: 0 ~ CANFD_MOTOR_NUM-1 */
bool canfd_get_motor_fb(uint8_t motor_idx, canfd_motor_fb_t * out);

/* 清空接收队列和统计 */
void canfd_rx_flush(void);

/* 可选：有 1ms 计数就在别处实现同名函数覆盖，掉线检测才会生效 */
uint32_t canfd_get_tick_ms(void);
/* ================== CAN 接收（新增）结束 ================== */

#endif //E_CAN_MIT_H
#include "hal_data.h"

extern uint8_t g_tx_busy;

fsp_err_t canfd_send_test_data(can_ctrl_t * p_ctrl, uint32_t id, uint8_t * p_data);
