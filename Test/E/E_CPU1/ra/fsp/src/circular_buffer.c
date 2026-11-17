//这是我的imu(陀螺仪)的循环缓冲区
//注意，看时要对照着IMU_System_t，CircularBuffer_t和IMU_Data_t三个结构体看，并且要注意，这三个结构体是有包含关系的
#include "circular_buffer.h"
#include "imu_system.h"
#include <stdlib.h>
IMU_Data_t imu_last_filtered;
static float filt_ax = 0.0f;
static float filt_ay = 0.0f;
static float filt_az = 0.0f;

static float filt_gx = 0.0f;
static float filt_gy = 0.0f;
static float filt_gz = 0.0f;

//这是初始化循环缓冲区，就是把成员全都初始化为0，目前，我已经在imu_i2c_driver.c定义了一个全局的缓冲区g_imu_system，
//但是他是IMU_System_t类型，所以，我们传入他的buffer成员就行，即，g_imu_system.buffer。IMU_System_t结构体在imu_system.c的头文件中
//********************************
void circular_buffer_init(CircularBuffer_t* cb)
{
    memset(cb, 0, sizeof(CircularBuffer_t));
}
//********************************
//这是向缓冲区写入数据IMU_Data_t类型的data数据，原理是操控head索引写入，IMU_BUFFER_SIZE是缓冲区大小，他在该文件的头文件中，可以根据实际硬件调整，目前值为100
// 向缓冲区写入数据，过滤跳变数据
bool circular_buffer_write(CircularBuffer_t* cb, const IMU_Data_t* data)
{
    uint32_t next_head = (cb->head + 1) % IMU_BUFFER_SIZE;
    IMU_Data_t filtered_data;

    if (cb->count == 0) {
        filtered_data = *data;
    } else {

        uint32_t last_idx = (cb->head - 1 + IMU_BUFFER_SIZE) % IMU_BUFFER_SIZE;
        const IMU_Data_t* last_data = &cb->data[last_idx];

        if (cb->count >= 3) {

            uint32_t prev_idx = (last_idx - 1 + IMU_BUFFER_SIZE) % IMU_BUFFER_SIZE;
            const IMU_Data_t* prev_data = &cb->data[prev_idx];

            filtered_data.accel_x = (( (data->accel_x - last_data->accel_x) * (last_data->accel_x - prev_data->accel_x) ) >= 0)
                ? data->accel_x
                : (abs(data->accel_x - last_data->accel_x) > MAXAX) ? last_data->accel_x : data->accel_x;

            filtered_data.accel_y = (( (data->accel_y - last_data->accel_y)  * (last_data->accel_y - prev_data->accel_y) ) >= 0)
                ? data->accel_y
                : (abs(data->accel_y - last_data->accel_y) > MAXAY) ? last_data->accel_y : data->accel_y;

            filtered_data.accel_z = (( (data->accel_z - last_data->accel_z)  * (last_data->accel_z - prev_data->accel_z) ) >= 0)
                ? data->accel_z
                : (abs(data->accel_z - last_data->accel_z) > MAXAZ) ? last_data->accel_z : data->accel_z;

            filtered_data.gyro_x = (( (data->gyro_x - last_data->gyro_x) * (last_data->gyro_x - prev_data->gyro_x) ) >= 0)
                ? data->gyro_x
                : (abs(data->gyro_x - last_data->gyro_x) > MAXJX) ? last_data->gyro_x : data->gyro_x;

            filtered_data.gyro_y = (( (data->gyro_y - last_data->gyro_y)  * (last_data->gyro_y - prev_data->gyro_y) ) >= 0)
                ? data->gyro_y
                : (abs(data->gyro_y - last_data->gyro_y) > MAXJY) ? last_data->gyro_y : data->gyro_y;

            filtered_data.gyro_z = (( (data->gyro_z - last_data->gyro_z)  * (last_data->gyro_z - prev_data->gyro_z) ) >= 0)
                ? data->gyro_z
                : (abs(data->gyro_z - last_data->gyro_z) > MAXJZ) ? last_data->gyro_z : data->gyro_z;
        }

        else {
            filtered_data.accel_x = (abs(data->accel_x - last_data->accel_x) > MAXAX) ? last_data->accel_x : data->accel_x;
            filtered_data.accel_y = (abs(data->accel_y - last_data->accel_y) > MAXAY) ? last_data->accel_y : data->accel_y;
            filtered_data.accel_z = (abs(data->accel_z - last_data->accel_z) > MAXAZ) ? last_data->accel_z : data->accel_z;
            filtered_data.gyro_x  = (abs(data->gyro_x  - last_data->gyro_x)  > MAXJX) ? last_data->gyro_x  : data->gyro_x;
            filtered_data.gyro_y  = (abs(data->gyro_y  - last_data->gyro_y)  > MAXJY) ? last_data->gyro_y  : data->gyro_y;
            filtered_data.gyro_z  = (abs(data->gyro_z  - last_data->gyro_z)  > MAXJZ) ? last_data->gyro_z  : data->gyro_z;
        }
    }

    if (next_head == cb->tail) {
        cb->overflow++;
        cb->tail = (cb->tail + 1) % IMU_BUFFER_SIZE;
        cb->count--;
    }

    memcpy(&cb->data[cb->head], &filtered_data, sizeof(IMU_Data_t));
    cb->head = next_head;
    cb->count++;
    cb->total++;

    return true;
}


    // 一阶低通滤波

IMU_Data_t imu_lowpass_filter(const IMU_Data_t* raw_data)
{
    filt_ax = (float)filt_ax * (1.0f - IMU_LPF_ALPHA) + raw_data->accel_x * IMU_LPF_ALPHA;
    filt_ay = filt_ay * (1.0f - IMU_LPF_ALPHA) + raw_data->accel_y * IMU_LPF_ALPHA;
    filt_az = filt_az * (1.0f - IMU_LPF_ALPHA) + raw_data->accel_z * IMU_LPF_ALPHA;

    // 增加陀螺滤波
    filt_gx = filt_gx * (1.0f - IMU_LPF_ALPHA) + raw_data->gyro_x * IMU_LPF_ALPHA;
    filt_gy = filt_gy * (1.0f - IMU_LPF_ALPHA) + raw_data->gyro_y * IMU_LPF_ALPHA;
    filt_gz = filt_gz * (1.0f - IMU_LPF_ALPHA) + raw_data->gyro_z * IMU_LPF_ALPHA;

    IMU_Data_t out = *raw_data;
    out.accel_x = (int16_t)filt_ax;
    out.accel_y = (int16_t)filt_ay;
    out.accel_z = (int16_t)filt_az;
    out.gyro_x  = (int16_t)filt_gx;
    out.gyro_y  = (int16_t)filt_gy;
    out.gyro_z  = (int16_t)filt_gz;
    return out;
}
//这是向缓冲区读最老的数据，原理是操控tail索引读取，看到cb->tail = (cb->tail + 1) % IMU_BUFFER_SIZE;没
//这说明使用他会导致tail移动到下一位，这就说明用他读一次最老数据就会丢失读到的最老数据
bool circular_buffer_read(CircularBuffer_t* cb, IMU_Data_t* data)
{
	//他是判断缓冲区是不是空的********
    if (cb->head == cb->tail) {
        return false;
    }
  //********************************
    memcpy(data, &cb->data[cb->tail], sizeof(IMU_Data_t));
    cb->tail = (cb->tail + 1) % IMU_BUFFER_SIZE;
    cb->count--;

    return true;
}
//********************************
//这是向缓冲区读最新的数据，原理是操控head索引读取，他就不用担心丢失数据，应为他不移动索引
bool circular_buffer_peek_latest(CircularBuffer_t* cb, IMU_Data_t* data)
{
    if (cb->head == cb->tail) {
        return false;
    }
    uint32_t latest = (cb->head == 0) ? IMU_BUFFER_SIZE - 1 : cb->head - 1;
    memcpy(data, &cb->data[latest], sizeof(IMU_Data_t));
    return true;
}
//********************************
//这是向缓冲区读取最多max_count个数据到buffer数组中，是从最老数据开始往前数max_count个
uint32_t imu_get_available_data(IMU_Data_t* buffer, uint32_t max_count)
{
    uint32_t available = circular_buffer_available(&g_imu_system.buffer);
    uint32_t to_read = (available < max_count) ? available : max_count;

    for (uint32_t i = 0; i < to_read; i++) {
        circular_buffer_read(&g_imu_system.buffer, &buffer[i]);
    }

    return to_read;
}
//这是向缓冲区读取还有多少数据，就是直接访问count成员
uint32_t circular_buffer_available(CircularBuffer_t* cb)
{
    return cb->count;
}