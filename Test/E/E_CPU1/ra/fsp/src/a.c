#include "hal_data.h"

#include <stdio.h>
#include <a.h>
FSP_CPP_HEADER
target_3d_t latest_targets1[MAX_TARGETS];
uint8_t      latest_count1;
/* 解析状态机枚举 */
typedef enum {
    PARSE_STATE_HEADER = 0,
    PARSE_STATE_COUNT,
    PARSE_STATE_DATA,
    PARSE_STATE_CHECKSUM,
    PARSE_STATE_TAIL
} parse_state_t;

/* =========================================================================
 * 全局变量区：用于存储最新数据，供机械臂主循环随时读取和打印
 * ========================================================================= */
target_3d_t g_latest_targets[MAX_TARGETS];
uint8_t     g_latest_count = 0;
uint8_t     rx_byte;
/* 新增：用于存储上一帧完整的原始字节流数据 */
uint8_t     g_latest_raw_frame[64];
uint8_t     g_latest_raw_len = 0;

/* 数据更新标志位：当收到全新一帧数据时，中断会将其置为 true */
bool g_new_data_ready = false;


/* =========================================================================
 * 核心解析函数：纯 C 状态机，同步记录原始字节流
 * ========================================================================= */
bool parse_blueberry_packet(uint8_t byte, target_3d_t *latest_targets, uint8_t *latest_count)
{
    static parse_state_t state = PARSE_STATE_HEADER;
    static uint8_t target_count = 0;
    static uint8_t data_buffer[60];
    static uint8_t data_index = 0;
    static uint8_t current_checksum = 0;

    /* 新增：用于在后台悄悄记录当前正在解析的这帧原始字节 */
    static uint8_t temp_raw_buf[64];
    static uint8_t temp_raw_len = 0;

    switch (state)
    {
        case PARSE_STATE_HEADER:
            if (byte == 0xAA) {
                state = PARSE_STATE_COUNT;
                current_checksum = 0;
                temp_raw_len = 0;                    // 帧头到来，清空临时缓存
                temp_raw_buf[temp_raw_len++] = byte; // 记录帧头 0xAA
            }
            break;

        case PARSE_STATE_COUNT:
            if (byte <= MAX_TARGETS) {
                temp_raw_buf[temp_raw_len++] = byte; // 记录目标数量
                target_count = byte;
                current_checksum += target_count;
                data_index = 0;
                if (target_count > 0) {
                    state = PARSE_STATE_DATA;
                } else {
                    state = PARSE_STATE_CHECKSUM;
                }
            } else {
                state = PARSE_STATE_HEADER;
            }
            break;

        case PARSE_STATE_DATA:
            temp_raw_buf[temp_raw_len++] = byte;     // 记录坐标数据字节
            data_buffer[data_index++] = byte;
            current_checksum += byte;

            if (data_index >= (target_count * 6)) {
                state = PARSE_STATE_CHECKSUM;
            }
            break;

        case PARSE_STATE_CHECKSUM:
            temp_raw_buf[temp_raw_len++] = byte;     // 记录校验和
            if (byte == current_checksum) {
                state = PARSE_STATE_TAIL;
            } else {
                state = PARSE_STATE_HEADER;
            }
            break;

        case PARSE_STATE_TAIL:
            temp_raw_buf[temp_raw_len++] = byte;     // 记录帧尾 0x55
            state = PARSE_STATE_HEADER;
            if (byte == 0x55) {
                /* 帧尾正确！开始解析字节并覆盖全局目标数组 */
                for (uint8_t i = 0; i < target_count; i++) {
                    uint8_t *p = &data_buffer[i * 6];
                    latest_targets[i].x_mm = (int16_t)((p[0] << 8) | p[1]);
                    latest_targets[i].y_mm = (int16_t)((p[2] << 8) | p[3]);
                    latest_targets[i].z_mm = (int16_t)((p[4] << 8) | p[5]);
                }
                *latest_count = target_count;

                /* 完美收齐！将临时记录的原始帧提取到全局变量中，供主循环打印 */
                for(uint8_t i = 0; i < temp_raw_len; i++) {
                    g_latest_raw_frame[i] = temp_raw_buf[i];
                }
                g_latest_raw_len = temp_raw_len;

                return true;
            }
            break;
    }
    return false;
}
