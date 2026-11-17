#include "S485.h"

uint8_t read_angle[];
uint8_t uart_rx_byte[1];
uint8_t rx_buf[20];
uint8_t rx_idx = 0;
motor_read_pos_t g_read_pos = {
    .address = 0x01,
    .position = 0
};
uint8_t uart_rx_byte[1];

// ========== 全局变量定义 ==========
motor_t g_motor = {
    .enable = {
        .address = 0x01,
        .enable = 0
    },
    .pid = {
        .address = 0x01,
        .kp = 300,
        .ki = 30,
        .kd = 380
    },
    .position = {
        .address = 0x01,
        .direction = 0,
        .acceleration = 600,
        .speed = 6000,
        .position = 0
    },

    .current_limit = {
        .address = 0x01,
        .current_ma = 5000
    }
};

// ========== UART回调函数 ==========
void red_motor_callback(uart_callback_args_t *p_args)
{
    // if (NULL == p_args) return;

    switch (p_args->event)
    {
        case UART_EVENT_TX_COMPLETE:
            break;
        case UART_EVENT_RX_COMPLETE:
            if (p_args->event != UART_EVENT_RX_COMPLETE) return;
            rx_buf[rx_idx++] = uart_rx_byte[0];
            if (rx_idx >= 8)
            {
                unpack_read_position_response(rx_buf, rx_idx, &g_read_pos);
                rx_idx = 0;
            }
            R_SCI_B_UART_Read(g_uart0.p_ctrl, uart_rx_byte, 1);
            break;

        case UART_EVENT_ERR_PARITY:
        case UART_EVENT_ERR_FRAMING:
        case UART_EVENT_ERR_OVERFLOW:
            break;
        default:
            break;
    }
}

// ========== CRC16计算函数 ==========
static uint16_t modbus_crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < buffer_length; i++)
    {
        crc ^= (uint16_t)buffer[i];

        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// ==========  PID参数设置指令打包函数 ==========
uint8_t pack_pid_command(motor_pid_t *cmd, uint8_t *buffer)
{
    if (cmd == NULL || buffer == NULL) return 0;
    uint8_t index = 0;
    buffer[index++] = cmd->address;
    buffer[index++] = 0x10;
    buffer[index++] = 0x00;
    buffer[index++] = 0x63;
    buffer[index++] = 0x00;
    buffer[index++] = 0x06;
    buffer[index++] = 0x0C;

    buffer[index++] = (uint8_t)(cmd->kp >> 24);
    buffer[index++] = (uint8_t)(cmd->kp >> 16);
    buffer[index++] = (uint8_t)(cmd->kp >> 8);
    buffer[index++] = (uint8_t)(cmd->kp & 0xFF);

    buffer[index++] = (uint8_t)(cmd->ki >> 24);
    buffer[index++] = (uint8_t)(cmd->ki >> 16);
    buffer[index++] = (uint8_t)(cmd->ki >> 8);
    buffer[index++] = (uint8_t)(cmd->ki & 0xFF);

    buffer[index++] = (uint8_t)(cmd->kd >> 24);
    buffer[index++] = (uint8_t)(cmd->kd >> 16);
    buffer[index++] = (uint8_t)(cmd->kd >> 8);
    buffer[index++] = (uint8_t)(cmd->kd & 0xFF);

    uint16_t crc = modbus_crc16(buffer, index);
    buffer[index++] = (uint8_t)(crc & 0xFF);
    buffer[index++] = (uint8_t)(crc >> 8);
    return index;
}
// ========== 速度控制指令打包函数 ==========
uint8_t pack_speed_command(motor_speed_t *cmd, uint8_t *buffer)
{
    if (cmd == NULL || buffer == NULL) return 0;

    uint8_t index = 0;

    buffer[index++] = cmd->address;
    buffer[index++] = 0x10;
    buffer[index++] = 0x00;
    buffer[index++] = 0xF1;
    buffer[index++] = 0x00;
    buffer[index++] = 0x03;
    buffer[index++] = 0x06;

    buffer[index++] = cmd->direction;
    buffer[index++] = cmd->acceleration;

    // float 转字节数组（大端序）
    uint8_t speed_bytes[4];
    memcpy(speed_bytes, &cmd->speed, 4);

    // 大端序输出
    buffer[index++] = speed_bytes[3];
    buffer[index++] = speed_bytes[2];
    buffer[index++] = speed_bytes[1];
    buffer[index++] = speed_bytes[0];

    uint16_t crc = modbus_crc16(buffer, index);
    buffer[index++] = (uint8_t)(crc & 0xFF);
    buffer[index++] = (uint8_t)(crc >> 8);

    return index;
}
// ========== 绝对位置控制指令打包函数 ==========
uint8_t pack_position_command(motor_position_t *cmd, uint8_t *buffer)
{
    if (cmd == NULL || buffer == NULL) return 0;
    uint8_t index = 0;
    buffer[index++] = cmd->address;
    buffer[index++] = 0x10;
    buffer[index++] = 0x00;
    buffer[index++] = 0xF2;
    buffer[index++] = 0x00;
    buffer[index++] = 0x04;
    buffer[index++] = 0x08;

    buffer[index++] = cmd->direction;
    buffer[index++] = cmd->acceleration;
    buffer[index++] = (uint8_t)(cmd->speed >> 8);
    buffer[index++] = (uint8_t)(cmd->speed & 0xFF);
    buffer[index++] = (uint8_t)(cmd->position >> 24);
    buffer[index++] = (uint8_t)(cmd->position >> 16);
    buffer[index++] = (uint8_t)(cmd->position >> 8);
    buffer[index++] = (uint8_t)(cmd->position & 0xFF);

    uint16_t crc = modbus_crc16(buffer, index);
    buffer[index++] = (uint8_t)(crc & 0xFF);
    buffer[index++] = (uint8_t)(crc >> 8);
    return index;
}
// ========== 电流限制设置指令打包函数 ==========

uint8_t pack_current_limit_command(motor_current_limit_t *cmd, uint8_t *buffer)
{
    if (cmd == NULL || buffer == NULL) return 0;
    uint8_t index = 0;

    buffer[index++] = cmd->address;
    buffer[index++] = 0x06;
    buffer[index++] = 0x00;
    buffer[index++] = 0x64;
    buffer[index++] = (uint8_t)(cmd->current_ma >> 8);
    buffer[index++] = (uint8_t)(cmd->current_ma & 0xFF);

    uint16_t crc = modbus_crc16(buffer, index);
    buffer[index++] = (uint8_t)(crc & 0xFF);
    buffer[index++] = (uint8_t)(crc >> 8);
    return index;
}

// ========== 读取实时位置指令打包函数 ==========
uint8_t pack_read_position_command(motor_read_pos_t *cmd, uint8_t *buffer)
{
    if (cmd == NULL || buffer == NULL) return 0;
    uint8_t index = 0;

    buffer[index++] = cmd->address;
    buffer[index++] = 0x04;
    buffer[index++] = 0x00;
    buffer[index++] = 0x2A;
    buffer[index++] = 0x00;
    buffer[index++] = 0x02;

    uint16_t crc = modbus_crc16(buffer, index);
    buffer[index++] = (uint8_t)(crc & 0xFF);
    buffer[index++] = (uint8_t)(crc >> 8);
    return index;
}

// ========== 解析驱动器返回的实时位置响应 ==========

uint8_t unpack_read_position_response(uint8_t *rx_buffer, uint8_t len, motor_read_pos_t *result)
{
    if (rx_buffer == NULL || result == NULL || len < 8) return 0;

    if (rx_buffer[0] != result->address || rx_buffer[1] != 0x04 || rx_buffer[2] != 0x04) return 0;


    result->position = ((uint32_t)rx_buffer[3] << 24) |
                       ((uint32_t)rx_buffer[4] << 16) |
                       ((uint32_t)rx_buffer[5] << 8)  |
                       ((uint32_t)rx_buffer[6]);

    return 1;
}
