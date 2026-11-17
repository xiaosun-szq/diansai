/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_ipc.h"
#include "r_sci_b_uart.h"
            #include "r_uart_api.h"
#include "r_ospi_b.h"
#include "r_spi_flash_api.h"
#include "r_capture_api.h"
            #include "r_ceu.h"
#include "r_sci_b_spi.h"
#include "r_spi_api.h"
FSP_HEADER
/** IPC Instance. */
            extern const ipc_instance_t g_ipc0;

            /** Access the IPC instance using these structures when calling API functions directly
            (::p_api is not used). */
            extern ipc_instance_ctrl_t g_ipc0_ctrl;
            extern const ipc_cfg_t g_ipc0_cfg;

#ifndef NULL
            void NULL(ipc_callback_args_t * p_args);
#endif
/** IPC Instance. */
            extern const ipc_instance_t g_ipc1;

            /** Access the IPC instance using these structures when calling API functions directly
            (::p_api is not used). */
            extern ipc_instance_ctrl_t g_ipc1_ctrl;
            extern const ipc_cfg_t g_ipc1_cfg;

#ifndef g_ipc1_callback
            void g_ipc1_callback(ipc_callback_args_t * p_args);
#endif
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart0;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_b_uart_instance_ctrl_t     g_uart0_ctrl;
            extern const uart_cfg_t g_uart0_cfg;
            extern const sci_b_uart_extended_cfg_t g_uart0_cfg_extend;

            #ifndef NULL
            void NULL(uart_callback_args_t * p_args);
            #endif
#if OSPI_B_CFG_DMAC_SUPPORT_ENABLE
    #include "r_dmac.h"
#endif
#if OSPI_CFG_DOTF_SUPPORT_ENABLE
    #include "r_sce_if.h"
#endif

extern const spi_flash_instance_t g_ospi0;
extern ospi_b_instance_ctrl_t g_ospi0_ctrl;
extern const spi_flash_cfg_t g_ospi0_cfg;
/* CEU on CAPTURE instance */
            extern const capture_instance_t g_ceu0;
            /* Access the CEU instance using these structures when calling API functions directly (::p_api is not used). */
            extern ceu_instance_ctrl_t g_ceu0_ctrl;
            extern const capture_cfg_t g_ceu0_cfg;
            #ifndef g_ceu0_user_callback
            void g_ceu0_user_callback(capture_callback_args_t * p_args);
            #endif
/** SPI on SCI Instance. */
extern const spi_instance_t g_sci_spi5;

/** Access the SCI_B_SPI instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_b_spi_instance_ctrl_t g_sci_spi5_ctrl;
extern const spi_cfg_t g_sci_spi5_cfg;

/** Called by the driver when a transfer has completed or an error has occurred (Must be implemented by the user). */
#ifndef sci5_b_spi_callback
void sci5_b_spi_callback(spi_callback_args_t * p_args);
#endif
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart9;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_b_uart_instance_ctrl_t     g_uart9_ctrl;
            extern const uart_cfg_t g_uart9_cfg;
            extern const sci_b_uart_extended_cfg_t g_uart9_cfg_extend;

            #ifndef NULL
            void NULL(uart_callback_args_t * p_args);
            #endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
