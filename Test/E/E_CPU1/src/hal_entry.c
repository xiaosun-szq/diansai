#include "hal_data.h"
#include "CAN_MIT.h"
#include "MIT_DBC.h"
#include "S485.h"
#include "UVW.h"
#include "os.h"
#include "SysTick.h"
#include "key_proc.h"
#include "imu_system.h"
#include "circular_buffer.h"
#include "imu_i2c_driver.h"
#include "foc_lib.h"
#include "a.h"
#include "arm_kinematics.h"
#include "blueberry_tracker.h"
#include <stdbool.h>
#include <stdint.h>


// ================= IPC 通信数据存储区 =================
// 【修复】：因为 OS.c 中已经定义了这三个变量，所以这里必须加 extern 声明！
extern volatile int16_t g_target_x;
extern volatile int16_t g_target_y;
extern volatile int16_t g_target_z;
extern volatile bool g_target_detected;
// ================= 明确大小的全局数组 =================
motor_position_t motor_position;
uint8_t buffer[15];
uint8_t moveBuf[];
uint8_t torqueBuf[];
uint8_t qr[] = {0x00, 0xFB,0xFB,0x00, 0xFB,0xFB,0x00, 0xFB};
uint8_t EN[8];

#if (1 == BSP_MULTICORE_PROJECT) && BSP_TZ_SECURE_BUILD
bsp_ipc_semaphore_handle_t g_core_start_semaphore = { .semaphore_num = 0 };
#endif

//********************************小核向大核发送函数********************************
__attribute__((section(".ram_noinit_nocache")))
static void send_msg(uint8_t reg_id, uint32_t data)
{
    uint32_t packed = ((uint32_t)reg_id << 24) | (data & 0x00FFFFFF);
    R_IPC_MessageSend(&g_ipc1_ctrl, packed);
}

// 防崩溃补丁：如果你打开了 g_ipc1，必须要有一个对应的回调函数，哪怕是空的
void g_ipc1_callback(ipc_callback_args_t *p_args)
{
    (void)p_args;
}
//********************************************************************************

//*****************************小核接收大核中断回调函数*****************************
void g_ipc0_callback(ipc_callback_args_t *p_args)
{
    uint32_t packed = p_args->message;
    uint8_t  reg_id = (packed >> 24) & 0xFF;
    uint32_t data   = packed & 0x00FFFFFF;
    int16_t value   = (int16_t)(data & 0xFFFF);

    switch (reg_id)
    {
        case 0x11: g_target_x = value; break; // 只接收X坐标
        case 0x12: g_target_y = value + 20; break; // 只接收Y坐标
        case 0x13: g_target_z = value; break; // 只接收Z坐标
        case 0x14: g_target_detected = (value == 1); break; // 0x14 决定当前是否真实看到蓝莓 (1=看到，0=丢失使用旧坐标)
        case 0x10: g_target_detected = false; break; // 兼容旧版的空指令
        default: break;
    }
}
//********************************************************************************

motor_state motor1 = { .add = 1, .steering = 0, .Brake = 1, .speed = 0.5f };
motor_state motor2 = { .add = 2, .steering = 0, .Brake = 1, .speed = 0.5f };
motor_state motor3 = { .add = 3, .steering = 0, .Brake = 1, .speed = 0.5f };
motor_state motor4 = { .add = 4, .steering = 0, .Brake = 1, .speed = 0.5f };

uint16_t l;
#include "r_i2c_master_api.h"

volatile bool i2c_tx_complete = false;
volatile bool i2c_rx_complete = false;
volatile bool i2c1_tx_complete = false;
volatile bool i2c1_rx_complete = false;
volatile bool i2c2_tx_complete = false;
volatile bool i2c2_rx_complete = false;
volatile bool xxxa = true;
volatile bool xxx1 = true;
volatile bool xxx2 = true;

void sci_b_spi_callback(spi_callback_args_t *p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event) { spi_transfer_complete = true; }
}

void sci_b_i2c_master_callback4(i2c_master_callback_args_t * p_args){
    if (p_args->event == I2C_MASTER_EVENT_TX_COMPLETE) {
        i2c1_tx_complete = true;
    } else if (p_args->event == I2C_MASTER_EVENT_RX_COMPLETE) {
        i2c1_rx_complete = true;
    } else if (p_args->event == I2C_MASTER_EVENT_ABORTED) {
        xxx1 = false;
    } else if (p_args->event == I2C_MASTER_EVENT_START) {
    } else{}
}

// 恢复 I2C5 的回调函数，防止 FOC 库底层报错
void sci_b_i2c_master_callback5(i2c_master_callback_args_t *p_args)
{
    if (p_args->event == I2C_MASTER_EVENT_TX_COMPLETE) {
        i2c2_tx_complete = true;
    } else if (p_args->event == I2C_MASTER_EVENT_RX_COMPLETE) {
        i2c2_rx_complete = true;
    } else if (p_args->event == I2C_MASTER_EVENT_ABORTED) {
        xxx2= false;
    }
}

void uart1_blueberry_callback (uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_TX_COMPLETE:  break;
        case UART_EVENT_RX_COMPLETE:
            g_uart1.p_api->read(g_uart1.p_ctrl, buf, sizeof(*buf));
            break;
        case UART_EVENT_ERR_PARITY:   break;
        case UART_EVENT_ERR_FRAMING:  break;
        case UART_EVENT_ERR_OVERFLOW: break;
        default: break;
    }
}


void hal_entry(void)
{
    SystickInit1();
    adc_status_t adc_sta;

#if (0 == _RA_CORE) && (1 == BSP_MULTICORE_PROJECT) && !BSP_TZ_NONSECURE_BUILD
#if BSP_TZ_SECURE_BUILD
    R_BSP_IpcSemaphoreTake(&g_core_start_semaphore);
#endif
    R_BSP_SecondaryCoreStart();
#if BSP_TZ_SECURE_BUILD
    while(FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&g_core_start_semaphore)) { ; }
#endif
#endif

    // 打开双向 IPC
    R_IPC_Open(&g_ipc0_ctrl, &g_ipc0_cfg);
    R_IPC_Open(&g_ipc1_ctrl, &g_ipc1_cfg);

    g_uart0.p_api->open(g_uart0.p_ctrl, g_uart0.p_cfg);
    g_uart1.p_api->open(g_uart1.p_ctrl, g_uart1.p_cfg);

    g_ioport.p_api->open(g_ioport.p_ctrl, g_ioport.p_cfg);

    R_BSP_PinWrite(BSP_IO_PORT_08_PIN_10 , 0);

    // ================= 修复：uart_rx_byte 是数组，不用加 & =================
    R_SCI_B_UART_Read(g_uart0.p_ctrl, uart_rx_byte, 1);

    // ================= ADC 初始化 =================
    R_ADC_B_Open(&g_adc0_ctrl, &g_adc0_cfg);
    R_ADC_B_ScanCfg(&g_adc0_ctrl, &g_adc0_scan_cfg);
    R_ADC_B_Calibrate(&g_adc0_ctrl, NULL);
    do {
        R_ADC_B_StatusGet(&g_adc0_ctrl, &adc_sta);
    } while (adc_sta.state == ADC_STATE_CALIBRATION_IN_PROGRESS);
    R_ADC_B_ScanStart(&g_adc0_ctrl);

    // ================= 定时器初始化 =================
    R_GPT_Open(&g_timer1_ctrl, &g_timer1_cfg);
    R_GPT_Start(&g_timer1_ctrl);
    R_GPT_Open(&g_timer2_ctrl, &g_timer2_cfg);
    R_GPT_Start(&g_timer2_ctrl);
    R_GPT_Open(&g_timer3_ctrl, &g_timer3_cfg);
    R_GPT_Start(&g_timer3_ctrl);

    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
    MIT_DBC_initialize();

    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
    g_i2c4.p_api->open(g_i2c4.p_ctrl, g_i2c4.p_cfg);

    g_uart0.p_api->write(g_uart0.p_ctrl, EN, 8);

    // ================= SPI 初始化 =================
    fsp_err_t err1 = g_sci_spi8.p_api->open(g_sci_spi8.p_ctrl, &g_sci_spi8_cfg);
    if(FSP_SUCCESS != err1) { while(1); }
    g_sci_spi8.p_api->callbackSet(g_sci_spi8.p_ctrl, sci_b_spi_callback, NULL, NULL);

    imu_system_init();
    circular_buffer_init(&g_imu_system.buffer);

    ArmController_Init(&arm_ctrl, 0.5f, 0.01f, 0.1f, 1.2f, 100.0f);
    Tracker_Init(&tracker, &arm_ctrl);
    R_IOPORT_PinCfg(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_04,
                     (uint32_t) (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CAN));
    R_IOPORT_PinCfg(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_05,
                     (uint32_t) (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CAN));
    R_CANFD_Open(&g_canfd1_ctrl, &g_canfd1_cfg);
    // ================= 开启全局中断 (IPC 就绪) =================
    __enable_irq();
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
    canfd_send_test_data((can_ctrl_t *)&g_canfd1_ctrl, 0x001, a);
    R_BSP_SoftwareDelay(3, BSP_DELAY_UNITS_MILLISECONDS);
    canfd_send_test_data((can_ctrl_t *)&g_canfd1_ctrl, 0x001, q);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    MIT_DBC_U3.Position = 0;
    MIT_DBC_step(&MIT_DBC_U3, &MIT_DBC_Y3);
    canfd_send_test_data(&g_canfd1_ctrl, 0x201, MIT_DBC_Y3.Out1);
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);

    g_uart0.p_api->write(g_uart0.p_ctrl, CLR_FAULT1, 5);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, DS1, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, EN1, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, p_moder1, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, CLR_POS1, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);


    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, DS2, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, EN2, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, p_moder2, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, CLR_POS2, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);

    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, DS3, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, EN3, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, p_moder3, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    g_uart0.p_api->write(g_uart0.p_ctrl, CLR_POS3, 8);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);

    R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
    g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_02, 1);
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
    g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_08_PIN_02, 0);

    g_uart1.p_api->read(g_uart1.p_ctrl, buf, sizeof(*buf));
    memcpy(&Angle, buf, sizeof(float));

    MATLAB_initialize();

    while (1) {

//        bool    has_target = g_target_detected;
//        int16_t target_x   = g_target_x;
//        int16_t target_y   = g_target_y;
//        int16_t target_z   = g_target_z;

       Os_Task();


    }

#if (1 == _RA_CORE) && (1 == BSP_MULTICORE_PROJECT) && BSP_TZ_SECURE_BUILD
    R_BSP_IpcSemaphoreGive(&g_core_start_semaphore);
#endif

#if BSP_TZ_SECURE_BUILD
    R_BSP_NonSecureEnter();
#endif
}

#if BSP_TZ_SECURE_BUILD
FSP_CPP_HEADER
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable () { }
FSP_CPP_FOOTER
#endif

void sci_i2c_txi_isr(void);
void sci_i2c_tei_isr(void);
#if SCI_I2C_CFG_DTC_ENABLE
void sci_i2c_rxi_isr(void);
#endif

const void *dummy_sci_i2c_isr_array[] = {
    sci_i2c_txi_isr,
    sci_i2c_tei_isr,
#if SCI_I2C_CFG_DTC_ENABLE
    sci_i2c_rxi_isr,
#endif
    (void *)0
};
