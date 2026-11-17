//
// Created by Qw011 on 2026/7/4.
//

#ifndef E_CPU1_OS_H
#define E_CPU1_OS_H

#endif //E_CPU1_OS_H
#ifndef __OS_H
#define __OS_H

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "MATLAB.h"
#include <stdlib.h>
#include <math.h>

#define true 1
#define false 0
#define PI 3.1415926535897932384626433832795

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
extern volatile float abc;
extern volatile uint8_t key_513_pressed;
extern volatile uint8_t key_311_pressed;
extern volatile uint8_t key_800_pressed;
extern volatile uint8_t key_906_pressed;
extern volatile uint8_t key_307_pressed;
extern uint8_t torqueBuf[];
extern float Angle;
extern uint8_t buf[];

extern volatile int16_t g_target_x;
extern volatile int16_t g_target_y;
extern volatile int16_t g_target_z;

extern void Os_Init(void);
extern void Os_Period(void);
extern void Os_Task(void);
extern volatile uint16_t adc_buf[8];
extern volatile uint8_t zero;
enum os_period {
    OS_0MS = 0u,
    OS_1MS = 1u,
    OS_5MS = 5u,
    OS_10MS = 10u,
    OS_50MS = 50u,
    OS_100MS = 100u,
    OS_500MS = 500u,
    OS_1000MS = 1000u
};

enum Os_Period_5ms {
    OS_5MS_a = 1u,
    OS_5MS_b = 2u,
    OS_5MS_c = 3u,
    OS_5MS_d = 4u,
    OS_5MS_e = 5u
};

void AAADDD(uint8_t ID, uint16_t angle, uint8_t *torqueBuf, uint8_t *moveBuf);

extern uint8_t a[8];
extern uint8_t u[8];
extern uint8_t b[8];
extern uint8_t qq[8];
extern uint8_t q[8];

extern uint8_t l1[];
extern uint8_t m1[];
extern uint8_t n1[];

extern uint8_t l2[];
extern uint8_t m2[];
extern uint8_t n2[];

extern uint8_t l3[];
extern uint8_t m3[];
extern uint8_t n3[];

extern uint8_t zero1[];

extern uint8_t p_moder1[];
extern uint8_t p_moder2[];
extern uint8_t p_moder3[];

extern uint8_t DS1[];
extern uint8_t DS2[];
extern uint8_t DS3[];

extern uint8_t EN1[];
extern uint8_t EN2[];
extern uint8_t EN3[];

extern uint8_t CLR_FAULT1[];
extern uint8_t CLR_FAULT2[];
extern uint8_t CLR_FAULT3[];

extern uint8_t CLR_POS1[];
extern uint8_t CLR_POS2[];
extern uint8_t CLR_POS3[];
extern uint8_t FeedbackData1[];
extern uint8_t FeedbackData2[];
extern uint8_t FeedbackData3[];
#endif
