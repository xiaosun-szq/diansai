//
// Created by Qw011 on 2026/6/28.
//
#include "hal_data.h"
#ifndef E_CPU1_UVW_H
#define E_CPU1_UVW_H

#endif //E_CPU1_UVW_H

typedef struct{
    uint8_t add;          // 几号电机
    uint8_t steering;     // 转向
    uint8_t Brake ;       // 刹车
    float speed;       // 速度百分比
} motor_state;

uint8_t motor_control(motor_state* motor_control);
