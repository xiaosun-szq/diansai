//
// Created by Qw011 on 2026/7/14.
//

#ifndef E_CPU1_IMU_SYSTEM_H
#define E_CPU1_IMU_SYSTEM_H

#endif //E_CPU1_IMU_SYSTEM_H
#ifndef IMU_SYSTEM_H
#define IMU_SYSTEM_H

#include <stdint.h>
#include <stdbool.h>
#include "circular_buffer.h"

typedef struct {
    CircularBuffer_t buffer;
    bool system_ready;          //标记 IMU 系统是否已完成初始化
    bool data_streaming;        //标记系统是否已正在持续采集数据，他两都是标志位，调试用的，写程序时关注buffer成员即可
} IMU_System_t;

extern IMU_System_t g_imu_system;

void imu_system_init(void);
void imu_system_start(void);
bool imu_get_latest_data(IMU_Data_t* data);

#endif