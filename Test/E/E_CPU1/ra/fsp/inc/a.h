//
// Created by Qw011 on 2026/7/19.
//

#ifndef E_CPU1_A_H
#define E_CPU1_A_H

#include <stdint.h>   // uint8_t int16_t
#include <stdbool.h>  // bool true false

#define MAX_TARGETS 10
/* 3D 坐标目标结构体 */
typedef struct {
    int16_t x_mm;
    int16_t y_mm;
    int16_t z_mm;
} target_3d_t;

// 蓝莓坐标结构体（解包后的最终数据）
/* 定义全局或静态的最新存储变量 */

extern target_3d_t latest_targets1[MAX_TARGETS];
extern uint8_t     latest_count1;
extern uint8_t     g_rx_berry_count;
extern uint8_t     rx_byte;
extern bool g_new_data_ready;

bool parse_blueberry_packet(uint8_t byte, target_3d_t *latest_targets, uint8_t *latest_count);
#endif //E_CPU1_A_H