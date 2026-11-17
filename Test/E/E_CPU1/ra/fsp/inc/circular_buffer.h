//
// Created by Qw011 on 2026/7/14.
//

#ifndef E_CPU1_CIRCULAR_BUFFER_H
#define E_CPU1_CIRCULAR_BUFFER_H

#endif //E_CPU1_CIRCULAR_BUFFER_H
#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#define IMU_LPF_ALPHA 0.2f

#define MAXAX 500    // accel_x 最大允许变化量
#define MAXAY 500    // accel_y 最大允许变化量
#define MAXAZ 500    // accel_z 最大允许变化量
#define MAXJX 500    // gyro_x 最大允许变化量
#define MAXJY 500    // gyro_y 最大允许变化量
#define MAXJZ 500    // gyro_z 最大允许变化量
// 环形缓冲区大小（根据实际硬件调整）
#define IMU_BUFFER_SIZE 100

// IMU数据结构体
typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    uint32_t timestamp;
} IMU_Data_t;

// 环形缓冲区结构体
typedef struct {
    IMU_Data_t data[IMU_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    uint32_t overflow;
    uint32_t total;
} CircularBuffer_t;

// 函数声明
void circular_buffer_init(CircularBuffer_t* cb);
bool circular_buffer_write(CircularBuffer_t* cb, const IMU_Data_t* data);
bool circular_buffer_read(CircularBuffer_t* cb, IMU_Data_t* data);
bool circular_buffer_peek_latest(CircularBuffer_t* cb, IMU_Data_t* data);
bool circular_buffer_peek_old(CircularBuffer_t* cb, IMU_Data_t* old_data);
uint32_t imu_get_available_data(IMU_Data_t* buffer, uint32_t max_count);
uint32_t circular_buffer_available(CircularBuffer_t* cb);
IMU_Data_t imu_lowpass_filter(const IMU_Data_t* raw_data);

#endif // CIRCULAR_BUFFER_H