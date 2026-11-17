//
// Created by Qw011 on 2026/7/14.
//

#ifndef E_CPU1_IMU_I2C_DRIVER_H
#define E_CPU1_IMU_I2C_DRIVER_H

#endif //E_CPU1_IMU_I2C_DRIVER_H
#ifndef IMU_I2C_DRIVER_H
#define IMU_I2C_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "imu_system.h"

bool imu_init(void);
void imu_read_data(void);
bool i2c_read(uint8_t read_id, uint8_t reg_addr, uint8_t *p_data, uint32_t len);

extern volatile uint32_t g_system_ms;
extern IMU_System_t g_imu_system;
extern volatile bool spi_transfer_complete;
extern volatile bool i2c1_tx_complete;
extern volatile bool i2c1_rx_complete;
extern volatile bool i2c2_tx_complete;
extern volatile bool i2c2_rx_complete;
extern IMU_Data_t imu_data;
extern uint8_t test[12];

bool spi_imu_write_byte(uint8_t reg_addr, uint8_t data);
bool spi_imu_read_bytes(uint8_t reg_addr, uint8_t *p_data, uint32_t len);
bool i2c_read_bytes1(uint8_t reg_addr, uint8_t *p_data, uint32_t len);
bool i2c_read_bytes2(uint8_t reg_addr, uint8_t *p_data, uint32_t len);
#endif