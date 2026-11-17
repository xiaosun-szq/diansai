//
// Created by Qw011 on 2026/7/14.
//
#include <string.h>include <stdbool.h>
#include "hal_data.h"
#include "imu_i2c_driver.h"
#include "os.h"
#include "foc_lib.h"
#include <stdio.h>   // sprintf/snprintf
#include <string.h>
volatile bool spi_transfer_complete = false;
volatile uint32_t g_system_ms = 0;//他是ms计数变量，我把他放在了os.c的Task_1ms()自增计数
IMU_System_t g_imu_system;        //他就是我们前面提到的全局循环缓冲区
IMU_Data_t imu_data;              //他是储存原始数据的
#define ICM42605_I2C_ADDR  0x68   //这是imu的iic地址
uint8_t test[12];

// SPI读取ICM42605寄存器，len为读取字节长度
bool spi_imu_read_bytes(uint8_t reg_addr, uint8_t *p_data, uint32_t len)
{
    fsp_err_t err;
    uint32_t timeout;
    uint8_t tx_buf[64] = {0};
    uint8_t rx_buf[64] = {0};

    if ((NULL == p_data) || (0 == len) || (len > 63)) {
        return false;
    }

    tx_buf[0] = 0x80 | reg_addr;
    for (uint32_t i = 1; i <= len; i++) {
        tx_buf[i] = 0x00;
    }

    g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_10, 0);
    spi_transfer_complete = false;

    err = g_sci_spi8.p_api->writeRead(
        g_sci_spi8.p_ctrl,
        tx_buf,
        rx_buf,
        len + 1,
        SPI_BIT_WIDTH_8_BITS
    );

    if (FSP_SUCCESS != err) {
        g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_10, 1);
        spi_transfer_complete = true;
        return false;
    }

    timeout = 1000;
    while (!spi_transfer_complete && (timeout-- > 0)) {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    }

    g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_10, 1);

    if (!spi_transfer_complete) {
        spi_transfer_complete = true;
        return false;
    }

    memcpy(p_data, &rx_buf[1], len);
    return true;
}
//// I²C读多个字节,他是给FOC角度读取用的
// bool i2c_read_bytes1(uint8_t reg_addr, uint8_t *p_data, uint32_t len)
//{
//    fsp_err_t err;
//    uint32_t timeout;
//
//    i2c1_tx_complete = false;
//
//    err = g_i2c4.p_api->write(g_i2c4.p_ctrl, &reg_addr, 1, false);
//    if (err != FSP_SUCCESS) return false;
//
//    timeout = 1000;
//    while (!i2c1_tx_complete && timeout--) {
//        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
//    }
//    if (!i2c1_tx_complete) return false;
//
//    i2c1_rx_complete = false;
//
//    err = g_i2c4.p_api->read(g_i2c4.p_ctrl, p_data, 1, false);
//    if (err != FSP_SUCCESS) return false;
//
//    timeout = 1000;
//    while (!i2c1_rx_complete && timeout--) {
//        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
//    }
//
//    return (i2c1_rx_complete);
//}
//// I²C读多个字节,他是给FOC角度读取用的
// bool i2c_read_bytes2(uint8_t reg_addr, uint8_t *p_data, uint32_t len)
//{
//    fsp_err_t err;
//    uint32_t timeout;
//
//    i2c2_tx_complete = false;
//
//    err = g_i2c5.p_api->write(g_i2c5.p_ctrl, &reg_addr, 1, false);
//    if (err != FSP_SUCCESS) return false;
//
//    timeout = 1000;
//    while (!i2c2_tx_complete && timeout--) {
//        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
//    }
//    if (!i2c2_tx_complete) return false;
//
//    i2c2_rx_complete = false;
//
//    err = g_i2c5.p_api->read(g_i2c5.p_ctrl, p_data, 1, false);
//    if (err != FSP_SUCCESS) return false;
//
//    timeout = 1000;
//    while (!i2c2_rx_complete && timeout--) {
//        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
//    }
//
//    return (i2c2_rx_complete);
//}
//*****************************************************
//原本我为了适配后续FOC双电机控制算法写了该函数，但后来优化过i2c_read函数就没用了，考虑到后续添加功能可能会用，就没删
 bool i2c_read(uint8_t read_id, uint8_t reg_addr, uint8_t *p_data, uint32_t len)
{
   switch(read_id) {
        case 1: i2c_read_bytes1(reg_addr, p_data,len); return true;
        case 2: i2c_read_bytes2(reg_addr, p_data,len); return true;
        default: return false;
    }
}
//*********************************************************************
// SPI写字节，思路和上面读是一样的
 bool spi_imu_write_byte(uint8_t reg_addr, uint8_t val)
 {
     fsp_err_t err;
     uint32_t timeout;
     uint8_t tx_buf[2] = {reg_addr, val};
     uint8_t rx_buf[2] = {0};

     g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_10, 0);
     spi_transfer_complete = false;

     err = g_sci_spi8.p_api->writeRead(
         g_sci_spi8.p_ctrl,
         tx_buf,
         rx_buf,
         2,
         SPI_BIT_WIDTH_8_BITS
     );

     if (FSP_SUCCESS != err) {
         g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_10, 1);
         spi_transfer_complete = true;
         return false;
     }

     timeout = 1000;
     while (!spi_transfer_complete && (timeout-- > 0)) {
         R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
     }

     g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_10, 1);

     if (!spi_transfer_complete) {
         spi_transfer_complete = true;
         return false;
     }

     return true;
 }
//*********************************************************************
//原始数据，这里是将读取到的test[12]整理为三方向加速度和三轴角速度
void imu_read_data(void)
{
    imu_data.accel_x = (int16_t)((test[0] << 8) | test[1]);
    imu_data.accel_y = (int16_t)((test[2] << 8) | test[3]);
    imu_data.accel_z = (int16_t)((test[4] << 8) | test[5]);

    imu_data.gyro_x = (int16_t)((test[6] << 8) | test[7]);
    imu_data.gyro_y = (int16_t)((test[8] << 8) | test[9]);
    imu_data.gyro_z = (int16_t)((test[10] << 8) | test[11]);

    imu_data.timestamp = g_system_ms;

    circular_buffer_write(&g_imu_system.buffer, &imu_data);//这是将刚刚读到的imu_data存入缓冲区，circular_buffer_write函数在circulay_buffer.c的第2个函数已经讲过了
}
