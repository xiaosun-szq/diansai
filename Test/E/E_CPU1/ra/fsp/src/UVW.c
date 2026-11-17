//
// Created by Qw011 on 2026/6/28.
//
#include "UVW.h"


uint8_t motor_control(motor_state* motor_control) {

    switch(motor_control->add){
        //*************************************************************************
        case (1):{

            g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_03_PIN_08, motor_control->steering);
            g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_05_PIN_12, motor_control->Brake);

            timer_info_t info;
            R_GPT_InfoGet(&g_timer1_ctrl, &info);
            R_GPT_DutyCycleSet(&g_timer1_ctrl, (uint32_t)(motor_control->speed * info.period_counts ), GPT_IO_PIN_GTIOCA);
            return 1;
        }
            //*************************************************************************
        case (2):	{
            g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_03_PIN_12, motor_control->steering);
            g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_05_PIN_11, motor_control->Brake);

            timer_info_t info;
            R_GPT_InfoGet(&g_timer1_ctrl, &info);
            R_GPT_DutyCycleSet(&g_timer1_ctrl, (uint32_t)(motor_control->speed * info.period_counts ), GPT_IO_PIN_GTIOCB);
            return 1;
        }
            //*************************************************************************
        case (3):{
            g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_03_PIN_10, motor_control->steering);
            g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_05, motor_control->Brake);

            timer_info_t info;
            R_GPT_InfoGet(&g_timer2_ctrl, &info);
            R_GPT_DutyCycleSet(&g_timer2_ctrl, (uint32_t)(motor_control->speed * info.period_counts ), GPT_IO_PIN_GTIOCA);
            return 1;
        }
            //*************************************************************************
        case (4):	{
            g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_09_PIN_05, motor_control->steering);
            g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_06, motor_control->Brake);

            timer_info_t info;
            R_GPT_InfoGet(&g_timer2_ctrl, &info);
            R_GPT_DutyCycleSet(&g_timer1_ctrl, (uint32_t)(motor_control->speed * info.period_counts ), GPT_IO_PIN_GTIOCB);
            return 1;
        }
        default:
            return 0;
    }

}