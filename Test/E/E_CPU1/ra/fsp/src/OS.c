#include "MIT_DBC.h"
#include "CAN_MIT.h"
#include "os.h"
#include <math.h>
#include "key_proc.h"
#include "dbc_proc.h"
#include "hal_data.h"
#include "S485.h"
#include "imu_system.h"
#include "circular_buffer.h"
#include "imu_i2c_driver.h"
#include "foc_lib.h"
#include "arm_kinematics.h"
#include "blueberry_tracker.h"

float Angle = 0;
uint8_t buf[];
// ======================== 【新增变量区域】 ========================
volatile uint32_t g_sys_tick = 0;         // 全局系统秒表(ms)，非阻塞延时的核心
volatile uint8_t  g_robot_state = 0;      // 状态机: 0-游离扫描, 1-锁定追踪, 2-执行抬升, 3-回拉复位
volatile uint32_t action_start_time = 0;  // 动作非阻塞计时器
volatile uint32_t no_near_count = 0;
volatile uint8_t wander_dir_x = 0;        // 游离态X轴弹珠碰撞方向
volatile uint8_t wander_dir_y = 1;        // 游离态Y轴弹珠碰撞方向

// 按键边沿检测状态记录 (用于非阻塞执行初始化的按键动作)
static uint8_t key_513_last = 1;
static uint8_t key_800_last = 1;
static uint8_t key_307_last = 1;
static uint8_t key_906_last = 1;
// =================================================================

float diff1 = 0;
volatile uint16_t t = 0;
volatile uint16_t delay_cnt = 0;
volatile uint16_t near_count = 0;
volatile uint8_t near_latched = 0;
volatile uint16_t out_of_range_count = 0;
volatile uint16_t window_index = 0;
volatile uint16_t window_count = 0;
volatile uint16_t update_counter = 0;
volatile float zero_position = 0.0f;

volatile uint16_t center_in_count = 0;
volatile uint16_t center_out_count = 0;
volatile uint8_t center_latched = 0;

volatile float current_position = 0.0f;
volatile int16_t g_target_x = 0;
volatile int16_t g_target_y = 0;
volatile int16_t g_target_z = 0;
volatile bool g_target_detected = false;

// 废弃原有的 x1, y1, nn, mm, ss 互斥锁，统一由状态机管理避免打架

uint8_t M1[15];
uint8_t M2[15];
uint8_t M3[15];

uint8_t VM1[17];
uint8_t VM2[17];
uint8_t VM3[17];

volatile float abc = 0;
volatile float tx = 0;
volatile float ty = 0;
volatile float tz = 0;

motor_position_t M11 = {
    .address = 0x01,
    .direction = 0,
    .acceleration = 200,
    .speed = 6000,
    .position = 0
};
motor_speed_t VM11 = {
	 .address = 0x01,
	 .direction = 0,
	 .acceleration = 200,
	 .speed = 0
};

motor_position_t M22 = {
    .address = 0x02,
    .direction = 1,
    .acceleration = 200,
    .speed = 6000,
    .position = 0
};
motor_speed_t VM22 = {
	 .address = 0x02,
	 .direction = 1,
	 .acceleration = 200,
	 .speed = 0
};

motor_position_t M33 = {
    .address = 0x03,
    .direction = 1,
    .acceleration = 200,
    .speed = 6000,
    .position = 0
};
motor_speed_t VM33 = {
	 .address = 0x03,
	 .direction = 1,
	 .acceleration = 200,
	 .speed = 0
};

float cvl = 0;
float lll = 0;
uint8_t rr[] = {0x00, 0x00, 0x33, 0x01};
timer_info_t info3;
IMU_Data_t data;
uint16_t DJ_angle=0;
float cvt =0;
uint8_t qqw= 0;
float atan2_deg(float y, float x)
{
    float rad = atan2f(y, x);
    return rad * 180.0f / PI;
}

//********************************横向1号电机P控制 (数学符号未作修改) ************************************
void P_motor1(void){
    if(g_target_x > 20&&g_target_x < 40){
    	VM11.direction = 0;
    	VM11.speed = g_target_x*15;
    }
    else if(g_target_x > -40&&g_target_x < -20){
    	VM11.direction = 1;
    	VM11.speed = -g_target_x*15;
    }
    else if(g_target_x < -40){
        VM11.direction = 1;
        VM11.speed = -g_target_x*70;
    }
    else if(g_target_x > 40){
        VM11.direction = 0;
        VM11.speed = g_target_x*70;
    }
    else if(g_target_x < 20&&g_target_x > 5){
        VM11.direction = 1;
        VM11.speed = -g_target_x*5;
    }
    else if(g_target_x > -20&&g_target_x < -5){
        VM11.direction = 0;
        VM11.speed = g_target_x*5;
    }
    else{
         VM11.speed = 0; // 速度为0，后续由统一发送引擎打包发送停止
    }
}

//********************************纵向23号电机P控制************************************
void P_motor2(void){
    if(g_target_y > 20&&g_target_y < 40){
    	VM22.direction = 0;
    	VM22.speed = g_target_y*15;
    }
    else if(g_target_y > -40&&g_target_y < -20){
    	VM22.direction = 1;
    	VM22.speed = -g_target_y*15;
    }
    else if(g_target_y < -40){
        VM22.direction = 1;
        VM22.speed = -g_target_y*70;
    }
    else if(g_target_y > 40){
        VM22.direction = 0;
        VM22.speed = g_target_y*70;
    }
    else if(g_target_y < 20&&g_target_y > 5){
        VM22.direction = 1;
        VM22.speed = -g_target_y*5;
    }
    else if(g_target_y > -20&&g_target_y < -5){
    	VM22.direction = 0;
    	VM22.speed = g_target_y*5;
    }
    else{
         VM22.speed = 0;
    }
}

void P_motor3(void){
    if(g_target_y > 20&&g_target_y < 40){
    	VM33.direction = 0;
    	VM33.speed = g_target_y*15;
    }
    else if(g_target_y > -40&&g_target_y < -20){
    	VM33.direction = 1;
    	VM33.speed = -g_target_y*15;
    }
    else if(g_target_y < -40){
    	VM33.direction = 1;
    	VM33.speed = -g_target_y*70;
    }
    else if(g_target_y > 40){
    	VM33.direction = 0;
    	VM33.speed = g_target_y*70;
    }
    else if(g_target_y < 20&&g_target_y > 5){
    	VM33.direction = 1;
        VM33.speed = -g_target_y*5;
    }
    else if(g_target_y > -20&&g_target_y < -5){
    	VM33.direction = 0;
    	VM33.speed = g_target_y*5;
    }
    else{
         VM33.speed = 0;
    }
}

//***********************************舵机转角控制************************************
void AAADDD(uint8_t ID, uint16_t angle, uint8_t *torqueBuf, uint8_t *moveBuf)
{
    torqueBuf[0] = 0xFF; torqueBuf[1] = 0xFF; torqueBuf[2] = ID;
    torqueBuf[3] = 0x04; torqueBuf[4] = 0x03; torqueBuf[5] = 0x18; torqueBuf[6] = 0x01;
    uint16_t sum_torque = ID + 0x04 + 0x03 + 0x18 + 0x01;
    torqueBuf[7] = (~sum_torque) & 0xFF;

    uint16_t pos = (uint16_t)round(1023.0f / 300.0f * angle);
    uint8_t pos_L = pos & 0xFF;
    uint8_t pos_H = (pos >> 8) & 0xFF;

    moveBuf[0] = 0xFF; moveBuf[1] = 0xFF; moveBuf[2] = ID;
    moveBuf[3] = 0x05; moveBuf[4] = 0x03; moveBuf[5] = 0x1E;
    moveBuf[6] = pos_L; moveBuf[7] = pos_H;
    uint16_t sum_move = ID + 0x05 + 0x03 + 0x1E + pos_L + pos_H;
    moveBuf[8] = (~sum_move) & 0xFF;
}

//***********************************按键防抖************************************
uint32_t qqq = 0; uint32_t k = 0; uint8_t f = 0; uint8_t c = 0; uint8_t stable_cnt = 0;
#define SAMPLE_STABLE_CNT 1
uint8_t KeyScan(uint8_t pin)
{
    uint8_t curr = pin;
    if (k % 10 == 0) {
        if ((f ^ curr) == 0) {
            stable_cnt++;
            if (stable_cnt >= SAMPLE_STABLE_CNT) {
                c = curr;
            }
        } else {
            stable_cnt = 0; f = curr;
        }
    }
    k++;
    return c;
}

uint8_t i = 0; uint8_t ii = 0; uint8_t p = 0;
uint8_t aaa = 0, bbb = 0, ccc = 0, aaa1 = 0, bbb1 = 0, ccc1= 0, ddd = 0, eee = 0, fff = 0, hh = 0;
uint32_t gg = 0;

volatile uint8_t w1[17];
volatile uint16_t adc_buf[8];
uint32_t r = 512000;
uint8_t a[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC};
uint8_t u[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD};
uint8_t b[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB};
uint8_t qq[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE};
uint8_t q[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA};

uint8_t l1[] = {0x01,0x10,0x00,0xF1,0x00,0x03,0x06,0x00,0xC8,0x45,0xBB,0x80,0x00,0x57,0xBF};
uint8_t m1[] = {0x01,0x10,0x00,0xF1,0x00,0x03,0x06,0x01,0xC8,0x45,0xBB,0x80,0x00,0x56,0x6E};
uint8_t n1[] = {0x01,0x10,0x00,0xF1,0x00,0x03,0x06,0x00,0xC8,0x00,0x00,0x00,0x00,0x53,0x56};

uint8_t l2[] = {0x02,0x10,0x00,0xF1,0x00,0x03,0x06,0x00,0xC8,0x45,0xBB,0x80,0x00,0x52,0x7C};
uint8_t m2[] = {0x02,0x10,0x00,0xF1,0x00,0x03,0x06,0x01,0xC8,0x45,0xBB,0x80,0x00,0x53,0xAD};
uint8_t n2[] = {0x02,0x10,0x00,0xF1,0x00,0x03,0x06,0x00,0xC8,0x00,0x00,0x00,0x00,0x56,0x95};

uint8_t l3[] = {0x03,0x10,0x00,0xF1,0x00,0x03,0x06,0x00,0xC8,0x45,0xBB,0x80,0x00,0x50,0xFD};
uint8_t m3[] = {0x03,0x10,0x00,0xF1,0x00,0x03,0x06,0x01,0xC8,0x45,0xBB,0x80,0x00,0x51,0x2C};
uint8_t n3[] = {0x03,0x10,0x00,0xF1,0x00,0x03,0x06,0x01,0xC8,0x00,0x00,0x00,0x00,0x55,0xC5};

uint8_t zero1[] = {0x01,0x10,0x00,0xF2,0x00,0x04,0x08,0x00,0xC8,0x17,0x70,0x00,0x00,0x00,0x00,0x07,0x9C};

uint8_t p_moder1[] = {0x01,0x06,0x00,0x62,0x00,0x00,0x28,0x14};
uint8_t p_moder2[] = {0x02,0x06,0x00,0x62,0x00,0x00,0x28,0x27};
uint8_t p_moder3[] = {0x03,0x06,0x00,0x62,0x00,0x00,0x29,0xF6};

uint8_t DS1[] = {0x01,0x06,0x00,0xFA,0x00,0x01,0x68,0x3B};
uint8_t DS2[] = {0x02,0x06,0x00,0xFA,0x00,0x01,0x68,0x08};
uint8_t DS3[] = {0x03,0x06,0x00,0xFA,0x00,0x01,0x69,0xD9};

uint8_t EN1[] = {0x01,0x06,0x00,0xFA,0x00,0x00,0xA9,0xFB};
uint8_t EN2[] = {0x02,0x06,0x00,0xFA,0x00,0x00,0xA9,0xC8};
uint8_t EN3[] = {0x03,0x06,0x00,0xFA,0x00,0x00,0xA8,0x19};

uint8_t CLR_FAULT1[] = {0x01,0x06,0x00,0xF8,0x00,0x01,0x39,0xFB};
uint8_t CLR_FAULT2[] = {0x02,0x06,0x00,0xFB,0x00,0x01,0x39,0xC8};
uint8_t CLR_FAULT3[] = {0x03,0x06,0x00,0xFB,0x00,0x01,0x38,0x19};

uint8_t CLR_POS1[] = {0x01,0x06,0x00,0xF8,0x00,0x01,0xC9,0xFB};
uint8_t CLR_POS2[] = {0x02,0x06,0x00,0xF8,0x00,0x01,0xC9,0xC8};
uint8_t CLR_POS3[] = {0x03,0x06,0x00,0xF8,0x00,0x01,0xC8,0x19};

enum os_period period = OS_0MS;

volatile uint8_t key_513_pressed;
volatile uint8_t key_311_pressed;
volatile uint8_t key_800_pressed;
volatile uint8_t key_906_pressed;
volatile uint8_t key_307_pressed;
volatile uint8_t key_804_pressed;
volatile uint8_t key_801_pressed;
volatile uint8_t key_908_pressed;
volatile uint8_t key_B06_pressed;
volatile uint8_t key_907_pressed;
volatile uint8_t key_909_pressed;

volatile uint8_t zero = 1;
uint16_t BalanceCarTaskCount = 0;
uint8_t Period_5ms_Count = 0;
uint8_t Period_10ms_Count = 0;
uint8_t Period_20ms_Count = 0;
uint8_t Period_100ms_Count = 0;
uint8_t Period_1000ms_Count = 0;

_Bool Period_1ms_Expire_Flag = false;
_Bool Period_5ms_Expire_Flag = false;
_Bool Period_10ms_Expire_Flag = false;
_Bool Period_20ms_Expire_Flag = false;
_Bool Period_100ms_Expire_Flag = false;
_Bool Period_1000ms_Expire_Flag = false;

static void Task_1ms(void);
static void Task_5ms(void);
static void Task_10ms(void);
static void Task_20ms(void);
static void Task_100ms(void);
static void Task_1000ms(void);


void Os_Period(void) {
    BalanceCarTaskCount++;
    Period_1ms_Expire_Flag = true;

    switch(BalanceCarTaskCount) {
        case OS_5MS_a:
            Period_5ms_Expire_Flag = true;
            break;
        case OS_5MS_b:
            if(Period_10ms_Count >= 2) {
                Period_10ms_Expire_Flag = true;
                Period_10ms_Count = 0;
            } else { Period_10ms_Count++; }
            break;
        case OS_5MS_c:
            if(Period_20ms_Count >= 4) {
                Period_20ms_Expire_Flag = true;
                Period_20ms_Count = 0;
            } else { Period_20ms_Count++; }
            break;
        case OS_5MS_d:
            if(Period_100ms_Count >= 20) {
                Period_100ms_Expire_Flag = true;
                Period_100ms_Count = 0;
            } else { Period_100ms_Count++; }
            break;
        case OS_5MS_e:
            BalanceCarTaskCount = 0;
            if(Period_1000ms_Count >= 200) {
                Period_1000ms_Expire_Flag = true;
                Period_1000ms_Count = 0;
            } else { Period_1000ms_Count++; }
            break;
        default: break;
    }
}

void Os_Task(void) {
    if(Period_1ms_Expire_Flag)   { Task_1ms(); Period_1ms_Expire_Flag = false; }
    if(Period_5ms_Expire_Flag)   { Task_5ms(); Period_5ms_Expire_Flag = false; }
    if(Period_10ms_Expire_Flag)  { Task_10ms(); Period_10ms_Expire_Flag = false; }
    if(Period_20ms_Expire_Flag)  { Task_20ms(); Period_20ms_Expire_Flag = false; }
    if(Period_100ms_Expire_Flag) { Task_100ms(); Period_100ms_Expire_Flag = false; }
    if(Period_1000ms_Expire_Flag){ Task_1000ms(); Period_1000ms_Expire_Flag = false; }
}

static void Task_1ms(void) {
    MATLAB_step();
    g_sys_tick++;
}


static void Task_5ms(void) {
    static uint8_t uart_seq = 0;

    if (uart_seq == 0) {
        if (VM11.speed == 0.0f) g_uart0.p_api->write(g_uart0.p_ctrl, n1, 15);
        else g_uart0.p_api->write(g_uart0.p_ctrl, VM1, pack_speed_command(&VM11, VM1));
        uart_seq = 1;
    }
    else if (uart_seq == 1) {
        if (VM22.speed == 0.0f) g_uart0.p_api->write(g_uart0.p_ctrl, n2, 15);
        else g_uart0.p_api->write(g_uart0.p_ctrl, VM2, pack_speed_command(&VM22, VM2));
        uart_seq = 2;
    }
    else if (uart_seq == 2) {
        if (VM33.speed == 0.0f) g_uart0.p_api->write(g_uart0.p_ctrl, n3, 15);
        else g_uart0.p_api->write(g_uart0.p_ctrl, VM3, pack_speed_command(&VM33, VM3));
        uart_seq = 0;
    }
    // MIT 关节电机每 5ms 无缝发送（避免超时失能）
    current_position = Angle;
    MIT_DBC_U1.Position = MATLAB_Y.Angle_out;
    MIT_DBC_step(&MIT_DBC_U1, &MIT_DBC_Y1);
    canfd_send_test_data(&g_canfd1_ctrl, 0x201, MIT_DBC_Y1.Out1);
}

static void Task_10ms(void) {
    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_08_PIN_00, (bsp_io_level_t *)&key_800_pressed);
    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_05_PIN_13, (bsp_io_level_t *)&key_513_pressed);
    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_06, (bsp_io_level_t *)&key_906_pressed);
    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_03_PIN_07, (bsp_io_level_t *)&key_307_pressed);
    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_08_PIN_04, (bsp_io_level_t *)&key_804_pressed);
    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_08_PIN_01, (bsp_io_level_t *)&key_801_pressed);
    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_08, (bsp_io_level_t *)&key_908_pressed);
    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_11_PIN_06, (bsp_io_level_t *)&key_B06_pressed);
    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_07, (bsp_io_level_t *)&key_907_pressed);
    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_09, (bsp_io_level_t *)&key_909_pressed);
}

static void Task_20ms(void) {
    // ----------- 1. 独立按键触发 (使用边沿检测，告别阻塞死延时) -----------
    // 只要按键从松开(1)变成按下(0)的瞬间触发一次通讯
    if (key_513_pressed == 0 && key_513_last == 1) { // 关节使能
        canfd_send_test_data((can_ctrl_t *)&g_canfd1_ctrl, 0x001, a);
    }
    key_513_last = key_513_pressed;

    if (key_800_pressed == 0 && key_800_last == 1) { // 清除关节错误
        canfd_send_test_data((can_ctrl_t *)&g_canfd1_ctrl, 0x001, q);

    }
    key_800_last = key_800_pressed;

    if (key_307_pressed == 0 && key_307_last == 1) { // 步进使能
        // 分时发送系统保证底层的稳定，直接调用即可
        g_uart0.p_api->write(g_uart0.p_ctrl, CLR_FAULT1, 8);
        g_uart0.p_api->write(g_uart0.p_ctrl, EN1, 8);
        g_uart0.p_api->write(g_uart0.p_ctrl, CLR_FAULT2, 8);
        g_uart0.p_api->write(g_uart0.p_ctrl, EN2, 8);
        g_uart0.p_api->write(g_uart0.p_ctrl, CLR_FAULT3, 8);
        g_uart0.p_api->write(g_uart0.p_ctrl, EN3, 8);
    }
    key_307_last = key_307_pressed;

    if (key_906_pressed == 0 && key_906_last == 1) { // 步进失能
        g_uart0.p_api->write(g_uart0.p_ctrl, DS1, 8);
        g_uart0.p_api->write(g_uart0.p_ctrl, DS2, 8);
        g_uart0.p_api->write(g_uart0.p_ctrl, DS3, 8);
    }
    key_906_last = key_906_pressed;

    // ----------- 2. 状态机运算层：只生成控制意图，绝对不发串口 -----------
    // ----------- 第二层：状态机运算层：只负责生成控制意图 -----------

        // ----------- 第三层：物理防撞极限优先层：拥有物理绝对覆写权 -----------
        // 只要压下按键，彻底无视上方所有状态意图，更新反弹方向并强行脱困
        if (key_907_pressed == 0) {
            wander_dir_x = 0;
            VM11.direction = 0; VM11.speed = 2000.0f;
            g_robot_state = 0; center_latched = 0; // 撞墙打断任何动作，清除锁存
        }
        if (key_909_pressed == 0) {
            wander_dir_x = 1;
            VM11.direction = 1; VM11.speed = 2000.0f;
            g_robot_state = 0; center_latched = 0;
        }
        if (key_801_pressed == 0) {
            wander_dir_y = 0;
            VM22.direction = 0; VM22.speed = 2000.0f;
            VM33.direction = 0; VM33.speed = 2000.0f;
            g_robot_state = 0; center_latched = 0;
        }
        if (key_908_pressed == 0) {
            wander_dir_y = 1;
            VM22.direction = 1; VM22.speed = 2000.0f;
            VM33.direction = 1; VM33.speed = 2000.0f;
            g_robot_state = 0; center_latched = 0;
        }

}


static void Task_100ms(void) {

}

static void Task_1000ms(void) {


}
