//
// Created by Qw011 on 2026/7/14.
//

#ifndef E_CPU1_FOC_LIB_H
#define E_CPU1_FOC_LIB_H

#endif //E_CPU1_FOC_LIB_H
#ifndef FOC_LIB_H
#define FOC_LIB_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#define PY                      0
// ==================== 配置参数（用户自定义） ====================
#define MOTOR_POLE_PAIRS        7      // 电机极对数
#define VOLTAGE_LIMIT           12.0f   // 最大输出电压 (V)
#define VOLTAGE_POWER_SUPPLY    24.0f   // 母线电压 (V)

// 速度环 PID 参数
#define SPEED1_PID_KP            4.0f  // 速度环比例系数
#define SPEED1_PID_KI            0.0f   // 速度环积分系数
#define SPEED1_PID_KD            0.0f  // 速度环微分系数
#define SPEED1_INTEGRAL_LIMIT    0.0f    // 速度环积分限幅
#define SPEED1_OUTPUT_LIMIT      12.0f   // 速度环输出限幅

#define SPEED2_PID_KP            0.133f  // 速度环比例系数
#define SPEED2_PID_KI            0.01f   // 速度环积分系数
#define SPEED2_PID_KD            0.001f  // 速度环微分系数
#define SPEED2_INTEGRAL_LIMIT    6.0f    // 速度环积分限幅
#define SPEED2_OUTPUT_LIMIT      12.0f   // 速度环输出限幅

// 位置环 PID 参数
#define POSITION1_PID_KP         3.0f    // 位置环比例系数
#define POSITION1_PID_KI         0.0f    // 位置环积分系数
#define POSITION1_PID_KD         0.5f    // 位置环微分系数
#define POSITION1_INTEGRAL_LIMIT 0.0f    // 位置环积分限幅
#define POSITION1_OUTPUT_LIMIT   12.0f   // 位置环输出限幅

#define POSITION2_PID_KP         14.0f    // 位置环比例系数
#define POSITION2_PID_KI         0.0f    // 位置环积分系数
#define POSITION2_PID_KD         0.2f    // 位置环微分系数
#define POSITION2_INTEGRAL_LIMIT 0.0f    // 位置环积分限幅
#define POSITION2_OUTPUT_LIMIT   12.0f   // 位置环输出限幅

// 电流环 PID 参数
#define CURRENT1_PID_KP          1.0f    // 电流环比例系数
#define CURRENT1_PID_KI          0.0f   // 电流环积分系数
#define CURRENT1_PID_KD          0.0f    // 电流环微分系数
#define CURRENT1_INTEGRAL_LIMIT  3.0f    // 电流环积分限幅
#define CURRENT1_OUTPUT_LIMIT    12.0f   // 电流环输出限幅

#define CURRENT2_PID_KP          20.0f    // 电流环比例系数
#define CURRENT2_PID_KI          0.0f   // 电流环积分系数
#define CURRENT2_PID_KD          0.0f    // 电流环微分系数
#define CURRENT2_INTEGRAL_LIMIT  3.0f    // 电流环积分限幅
#define CURRENT2_OUTPUT_LIMIT    12.0f   // 电流环输出限幅

#define LPF_ALPHA               0.1f    // 低通滤波系数
#define PI                      3.141592653589793f

#define _constrain(amt, low, high) \
    ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

extern volatile uint32_t us_counter;
extern volatile uint32_t ms_counter;

// ==================== PID 控制器结构体 ====================
typedef struct {
    float Kp;                    // 比例系数
    float Ki;                    // 积分系数
    float Kd;                    // 微分系数

    float integral;              // 积分累计值
    float error_prev;            // 上一次误差（用于微分）
    float D_prev;                // 上一次D项输出（用于微分滤波）

    float integral_limit;        // 积分限幅
    float output_limit;          // 输出限幅

    unsigned long timestamp_prev;// 上一次更新时间戳（微秒）
} PIDController_t;

extern PIDController_t speed_pid[3];
extern PIDController_t position_pid[3];
extern PIDController_t current_pid[3];
// ==================== PID 控制器函数 ====================
void PID_Init(PIDController_t *pid, float Kp, float Ki, float Kd,
              float integral_limit, float output_limit);                        // 初始化 PID 控制器

float PID_Update(PIDController_t *pid, float error, unsigned long timestamp);   // PID 更新（位置式），返回控制量

float PID_Update_Incremental(PIDController_t *pid, float error, unsigned long timestamp); // PID 更新（增量式），返回控制增量

void PID_Reset(PIDController_t *pid);                                           // 重置 PID 内部状态（清零积分、误差）

// ==================== 编码器函数（AS5600） ====================
float getSensorAngle(uint8_t a);                                                // 获取原始数据
float getAngle_Without_track(uint8_t a);                                        // 获取无圈数角度（±π范围）
float getAngle(uint8_t a);                                                      // 获取累计角度（含圈数）
float Sensor_AS5600_getVelocity(uint8_t a,unsigned long timestamp);             // 计算角速度（微秒时间戳）

// ==================== 定时器函数 ====================
uint32_t micros(void);                                                          // 获取当前微秒数
uint32_t millis(void);                                                          // 获取当前毫秒数

// ==================== PWM输出函数 ====================
void hal_pwm_init(void);                                                        // 初始化三相PWM
void setPwm_motor1(float U1a, float U1b, float U1c);
void setPwm_motor2(float U2a, float U2b, float U2c);
void setPwm(uint8_t motor_id,float Ua, float Ub, float Uc);                     // 设置三相占空比

// ==================== FOC数学函数 ====================
float _normalizeAngle(float angle);                                             // 角度归一化到 0~2π
float _electricalAngle(float mechanical_angle);                                 // 机械角转电角
void setPhaseVoltage(uint8_t a, float Uq, float Ud, float angle_el);            // 电压矢量 → 三相电压
float ThreePhase2Iq(float Ia, float Ib, float Ic,
                    float theta_e);                                             // 三相电流 → Iq

// ==================== 滤波器函数 ====================
float LowPassFilter_operator(float x, unsigned long timestamp, float lpf_y_prev
	                           , unsigned long lpf_timestamp_prev);               // 一阶低通滤波

// ==================== 开环速度控制 ==================
float velocityOpenloop(uint8_t a, float target_velocity);                       // 角速度速度生成

// ==================== 控制环函数 ====================
void velocityControlLoop(uint8_t a, float target_velocity, float current_velocity,
                         float angle_el, unsigned long timestamp,
                         PIDController_t *pid);                                 // 速度环（带PID参数）

void positionControlLoop(uint8_t a, float target_position, float current_position,
                         float angle_el, unsigned long timestamp,
                         PIDController_t *pid);                                 // 位置环（带PID参数）

void currentControlLoop(uint8_t a, float target_iq, float current_iq,
                        float angle_el, unsigned long timestamp,
                        PIDController_t *pid);                                  // 电流环（预留）

// ==================== 应用层函数 ====================
void motor_app_init(void);                                                      // 电机应用初始化
void motor_app_loop(uint8_t a, float target_speed);                             // 电机主循环

												float _electricalAngle(float mechanical_angle);

#endif