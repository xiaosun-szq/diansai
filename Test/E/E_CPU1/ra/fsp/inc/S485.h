#ifndef S485_H
#define S485_H

#include <stdint.h>
#include "hal_data.h"
#include "r_uart_api.h"
#include "r_sci_b_uart.h"

// ========== 1. 使能控制结构体 ==========
typedef struct {
    uint8_t address;      // 从机地址 (默认0x01)
    uint8_t enable;       // 0=使能, 1=失能
} motor_enable_t;

typedef struct {
    uint8_t address;       // 从机地址 (1~255)
    uint8_t direction;     // 旋转方向 0=正转, 1=反转
    uint8_t acceleration;  // 加减速度 0~200 (单位r/ss，0=直接启动)
    float   speed;         // 目标速度 0.1~6000 RPM
} motor_speed_t;
// ========== 2. PID参数结构体 ==========
typedef struct {
    uint8_t address;      // 从机地址 (默认0x01)
    uint32_t kp;          // 比例系数
    uint32_t ki;          // 积分系数
    uint32_t kd;          // 微分系数
} motor_pid_t;

// ========== 3. 绝对位置控制结构体 ==========
typedef struct {
    uint8_t address;      // 从机地址 (默认0x01)
    uint8_t direction;    // 0=正转, 1=反转
    uint16_t acceleration; // 0-600, 0=直接启动
    uint16_t speed;       // 0-6000 RPM
    uint32_t position;    // 51200 = 1圈
} motor_position_t;

// ========== 4. 零点设置结构体 ==========
typedef struct {
    uint8_t address;      // 从机地址 (默认0x01)
    uint32_t position;    // 原点位置 (51200 = 1圈)
} motor_home_t;

// ========== 5.电流限制结构体 ==========
typedef struct {
    uint8_t address;      // 从机地址 (默认0x01)
    uint16_t current_ma;  // 电流限制 (0~3000 mA)
} motor_current_limit_t;

// ========== 6. 读取实时角度结构体 ==========
typedef struct {
    uint8_t address;      // 从机地址 (默认0x01)
    uint32_t position;    // 读取到的实时位置
} motor_read_pos_t;

// ========== 打包函数声明 ==========
uint8_t pack_pid_command(motor_pid_t *cmd, uint8_t *buffer);
uint8_t pack_position_command(motor_position_t *cmd, uint8_t *buffer);
uint8_t pack_speed_command(motor_speed_t *cmd, uint8_t *buffer);
uint8_t pack_home_command(motor_home_t *cmd, uint8_t *buffer);


uint8_t pack_current_limit_command(motor_current_limit_t *cmd, uint8_t *buffer);
uint8_t pack_read_position_command(motor_read_pos_t *cmd, uint8_t *buffer);
uint8_t unpack_read_position_response(uint8_t *rx_buffer, uint8_t len, motor_read_pos_t *result);
uint8_t pack_atk_speed_command(uint8_t address, uint8_t direction, uint8_t accel, float speed, uint8_t *buffer);
// ========== 总控结构体 ==========
typedef struct {
    motor_enable_t enable;
    motor_pid_t pid;
    motor_position_t position;
    motor_home_t home;
    motor_current_limit_t current_limit;
} motor_t;

extern motor_t g_motor;
extern uint8_t read_angle[];
extern uint8_t uart_rx_byte[1];
extern motor_read_pos_t g_read_pos;
#endif /* S485_H */
