/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_sci_b_spi.h"
#include "r_spi_api.h"
#include "r_sci_b_i2c.h"
#include "r_i2c_master_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_sci_b_uart.h"
            #include "r_uart_api.h"
#include "r_adc_b.h"
                      #include "r_adc_api.h"
#include "r_canfd.h"
#include "r_can_api.h"
#include "r_ipc.h"
FSP_HEADER
/** SPI on SCI Instance. */
extern const spi_instance_t g_sci_spi8;

/** Access the SCI_B_SPI instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_b_spi_instance_ctrl_t g_sci_spi8_ctrl;
extern const spi_cfg_t g_sci_spi8_cfg;

/** Called by the driver when a transfer has completed or an error has occurred (Must be implemented by the user). */
#ifndef sci_b_spi_callback
void sci_b_spi_callback(spi_callback_args_t * p_args);
#endif
extern const i2c_master_cfg_t g_i2c5_cfg;
/* I2C on SCI Instance. */
extern const i2c_master_instance_t g_i2c5;
#ifndef sci_b_i2c_master_callback5
void sci_b_i2c_master_callback5(i2c_master_callback_args_t * p_args);
#endif

extern const sci_b_i2c_extended_cfg_t g_i2c5_cfg_extend;
extern sci_b_i2c_instance_ctrl_t g_i2c5_ctrl;
extern const i2c_master_cfg_t g_i2c4_cfg;
/* I2C on SCI Instance. */
extern const i2c_master_instance_t g_i2c4;
#ifndef sci_b_i2c_master_callback4
void sci_b_i2c_master_callback4(i2c_master_callback_args_t * p_args);
#endif

extern const sci_b_i2c_extended_cfg_t g_i2c4_cfg_extend;
extern sci_b_i2c_instance_ctrl_t g_i2c4_ctrl;
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer13;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer13_ctrl;
extern const timer_cfg_t g_timer13_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer11;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer11_ctrl;
extern const timer_cfg_t g_timer11_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer9;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer9_ctrl;
extern const timer_cfg_t g_timer9_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer8;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer8_ctrl;
extern const timer_cfg_t g_timer8_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer7;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer7_ctrl;
extern const timer_cfg_t g_timer7_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart1;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_b_uart_instance_ctrl_t     g_uart1_ctrl;
            extern const uart_cfg_t g_uart1_cfg;
            extern const sci_b_uart_extended_cfg_t g_uart1_cfg_extend;

            #ifndef uart1_blueberry_callback
            void uart1_blueberry_callback(uart_callback_args_t * p_args);
            #endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer3;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer3_ctrl;
extern const timer_cfg_t g_timer3_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** ADC on ADC_B instance. */
                    extern const adc_instance_t g_adc0;

                    /** Access the ADC_B instance using these structures when calling API functions directly (::p_api is not used). */
                    extern adc_b_instance_ctrl_t g_adc0_ctrl;
                    extern const adc_cfg_t g_adc0_cfg;
                    extern const adc_b_scan_cfg_t g_adc0_scan_cfg;

                    #ifndef NULL
                    void NULL(adc_callback_args_t * p_args);
                    #endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer2;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer2_ctrl;
extern const timer_cfg_t g_timer2_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart0;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_b_uart_instance_ctrl_t     g_uart0_ctrl;
            extern const uart_cfg_t g_uart0_cfg;
            extern const sci_b_uart_extended_cfg_t g_uart0_cfg_extend;

            #ifndef red_motor_callback
            void red_motor_callback(uart_callback_args_t * p_args);
            #endif
/** CANFD on CANFD Instance. */
extern const can_instance_t g_canfd1;
/** Access the CANFD instance using these structures when calling API functions directly (::p_api is not used). */
extern canfd_instance_ctrl_t g_canfd1_ctrl;
extern const can_cfg_t g_canfd1_cfg;
extern const canfd_extended_cfg_t g_canfd1_cfg_extend;

#ifndef canfd0_callback
void canfd0_callback(can_callback_args_t * p_args);
#endif

/* Global configuration (referenced by all instances) */
extern canfd_global_cfg_t g_canfd_global_cfg;
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer1;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer1_ctrl;
extern const timer_cfg_t g_timer1_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** IPC Instance. */
            extern const ipc_instance_t g_ipc1;

            /** Access the IPC instance using these structures when calling API functions directly
            (::p_api is not used). */
            extern ipc_instance_ctrl_t g_ipc1_ctrl;
            extern const ipc_cfg_t g_ipc1_cfg;

#ifndef NULL
            void NULL(ipc_callback_args_t * p_args);
#endif
/** IPC Instance. */
            extern const ipc_instance_t g_ipc0;

            /** Access the IPC instance using these structures when calling API functions directly
            (::p_api is not used). */
            extern ipc_instance_ctrl_t g_ipc0_ctrl;
            extern const ipc_cfg_t g_ipc0_cfg;

#ifndef g_ipc0_callback
            void g_ipc0_callback(ipc_callback_args_t * p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
