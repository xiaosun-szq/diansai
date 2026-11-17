
#include "hal_data.h"
#include "imu_system.h"
#include "imu_i2c_driver.h"

//这是imu的通讯配置，参考ICM42605数据手册
void imu_system_init(void)
{
    uint8_t reg_val;
    uint16_t retry_cnt = 0;
    const uint16_t MAX_RETRY = 1000;

    memset(&g_imu_system, 0, sizeof(g_imu_system));
    circular_buffer_init(&g_imu_system.buffer);

    // ===== 1. 软复位 (Section 14.1 DEVICE_CONFIG) =====
    // SOFT_RESET_CONFIG = 1，复位后等待1ms
    spi_imu_write_byte(0x11, 0x01);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

    // ===== 2. 校验WHO_AM_I (Section 14.58) =====
    // WHO_AM_I (0x75) 默认值 = 0x42
    while (1) {
        bool read_ok = spi_imu_read_bytes(0x75, &reg_val, 1);
        if (read_ok && (0x42 == reg_val)) {
            break;  // 芯片识别成功
        }

        retry_cnt++;
        if (retry_cnt >= MAX_RETRY) {
            break;
            // 芯片不存在或SPI通信异常，死循环报错
            // while (1);
        }
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }

    // ===== 3. ️ 修复INT引脚配置 (Section 12.5) =====
    // INT_CONFIG1 (0x64) bit4 INT_ASYNC_RESET 必须从默认1改为0
    // 否则INT1/INT2引脚工作异常
    spi_imu_write_byte(0x64, 0x00);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

    // ===== 4. 电源管理 (Section 14.36 PWR_MGMT0) =====
    // 0x0F = GYRO_MODE=LN(3), ACCEL_MODE=LN(3)，六轴低噪声模式
    spi_imu_write_byte(0x4E, 0x0F);
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    // ===== 5. 陀螺仪配置 (Section 14.37 GYRO_CONFIG0) =====
    // 0x06 = GYRO_FS_SEL=000(±2000dps), GYRO_ODR=0110(1kHz)
    spi_imu_write_byte(0x4F, 0x06);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

    // ===== 6. 加速度计配置 (Section 14.38 ACCEL_CONFIG0) =====
    // 0x06 = ACCEL_FS_SEL=000(±16g), ACCEL_ODR=0110(1kHz)
    spi_imu_write_byte(0x50, 0x06);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

    // ===== 7. FIFO配置 (Section 14.4 FIFO_CONFIG) =====
    // 0x00 = Bypass Mode，禁用FIFO
    spi_imu_write_byte(0x16, 0x00);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

    g_imu_system.system_ready = true;
}
//设定缓冲区g_imu_system的data_streaming成员值，相当于置标志为，调试用，不用管他
void imu_system_start(void)
{
    g_imu_system.data_streaming = true;
}

//读取缓冲区最新数据，保存到data中，本质就是调用了circular_buffer_peek_latest函数(circular_buffer.c说过的)
bool imu_get_latest_data(IMU_Data_t* data)
{
    if (!g_imu_system.system_ready) return false;
    return circular_buffer_peek_latest(&g_imu_system.buffer, data);
}

