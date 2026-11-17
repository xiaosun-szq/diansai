//
// Created by Qw011 on 2026/7/4.
//
#include "SysTick.h"
#include "os.h"
#include "blueberry_tracker.h"

fsp_err_t SystickInit1(void) {
    uint32_t uwSysclk = R_BSP_SourceClockHzGet(FSP_PRIV_CLOCK_PLL);
    if (SysTick_Config(uwSysclk / 4000) != 0) {
        return FSP_ERR_ASSERTION;
    }
    return FSP_SUCCESS;
}

void SysTick_Handler(void) {
    Os_Period();
    // tracker.align_stable_cnt++;
    // tracker.picking_timer++;
}


