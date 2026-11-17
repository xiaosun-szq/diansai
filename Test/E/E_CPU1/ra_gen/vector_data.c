/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = ipc_isr, /* IPC IRQ0 (CPU Mutual Interrupt 0) */
            [1] = canfd_error_isr, /* CAN1 CHERR (Channel  error) */
            [2] = canfd_channel_tx_isr, /* CAN1 TX (Transmit interrupt) */
            [3] = canfd_common_fifo_rx_isr, /* CAN1 COMFRX (Common FIFO receive interrupt) */
            [4] = canfd_error_isr, /* CAN GLERR (Global error) */
            [5] = canfd_rx_fifo_isr, /* CAN RXF (Global receive FIFO interrupt) */
            [6] = sci_b_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
            [7] = sci_b_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [8] = sci_b_uart_tei_isr, /* SCI0 TEI (Transmit end) */
            [9] = sci_b_uart_eri_isr, /* SCI0 ERI (Receive error) */
            [10] = adc_b_limclpi_isr, /* ADC LIMCLPI (Limiter clip interrupt with the limit table 0 to 7) */
            [11] = adc_b_err0_isr, /* ADC ERR0 (A/D converter unit 0 Error) */
            [12] = adc_b_err1_isr, /* ADC ERR1 (A/D converter unit 1 Error) */
            [13] = adc_b_resovf0_isr, /* ADC RESOVF0 (A/D conversion overflow on A/D converter unit 0) */
            [14] = adc_b_resovf1_isr, /* ADC RESOVF1 (A/D conversion overflow on A/D converter unit 1) */
            [15] = adc_b_calend0_isr, /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
            [16] = adc_b_calend1_isr, /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
            [17] = adc_b_adi0_isr, /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
            [18] = adc_b_adi1_isr, /* ADC ADI1 (End of A/D scanning operation(Gr.1)) */
            [19] = adc_b_adi2_isr, /* ADC ADI2 (End of A/D scanning operation(Gr.2)) */
            [20] = adc_b_adi3_isr, /* ADC ADI3 (End of A/D scanning operation(Gr.3)) */
            [21] = adc_b_adi4_isr, /* ADC ADI4 (End of A/D scanning operation(Gr.4)) */
            [22] = adc_b_fifoovf_isr, /* ADC FIFOOVF (FIFO data overflow) */
            [23] = adc_b_fiforeq0_isr, /* ADC FIFOREQ0 (FIFO data read request interrupt(Gr.0)) */
            [24] = adc_b_fiforeq1_isr, /* ADC FIFOREQ1 (FIFO data read request interrupt(Gr.1)) */
            [25] = adc_b_fiforeq2_isr, /* ADC FIFOREQ2 (FIFO data read request interrupt(Gr.2)) */
            [26] = adc_b_fiforeq3_isr, /* ADC FIFOREQ3 (FIFO data read request interrupt(Gr.3)) */
            [27] = adc_b_fiforeq4_isr, /* ADC FIFOREQ4 (FIFO data read request interrupt(Gr.4)) */
            [28] = sci_b_uart_rxi_isr, /* SCI1 RXI (Receive data full) */
            [29] = sci_b_uart_txi_isr, /* SCI1 TXI (Transmit data empty) */
            [30] = sci_b_uart_tei_isr, /* SCI1 TEI (Transmit end) */
            [31] = sci_b_uart_eri_isr, /* SCI1 ERI (Receive error) */
            [32] = sci_b_i2c_txi_isr, /* SCI4 TXI (Transmit data empty) */
            [33] = sci_b_i2c_tei_isr, /* SCI4 TEI (Transmit end) */
            [34] = sci_b_i2c_txi_isr, /* SCI5 TXI (Transmit data empty) */
            [35] = sci_b_i2c_tei_isr, /* SCI5 TEI (Transmit end) */
            [36] = sci_b_spi_rxi_isr, /* SCI8 RXI (Receive data full) */
            [37] = sci_b_spi_txi_isr, /* SCI8 TXI (Transmit data empty) */
            [38] = sci_b_spi_tei_isr, /* SCI8 TEI (Transmit end) */
            [39] = sci_b_spi_eri_isr, /* SCI8 ERI (Receive error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_IPC_IRQ0,GROUP0), /* IPC IRQ0 (CPU Mutual Interrupt 0) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_CAN1_CHERR,GROUP1), /* CAN1 CHERR (Channel  error) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_CAN1_TX,GROUP2), /* CAN1 TX (Transmit interrupt) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_CAN1_COMFRX,GROUP3), /* CAN1 COMFRX (Common FIFO receive interrupt) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_CAN_GLERR,GROUP4), /* CAN GLERR (Global error) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_CAN_RXF,GROUP5), /* CAN RXF (Global receive FIFO interrupt) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_SCI0_RXI,GROUP6), /* SCI0 RXI (Receive data full) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TXI,GROUP7), /* SCI0 TXI (Transmit data empty) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TEI,GROUP0), /* SCI0 TEI (Transmit end) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI0_ERI,GROUP1), /* SCI0 ERI (Receive error) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_ADC_LIMCLPI,GROUP2), /* ADC LIMCLPI (Limiter clip interrupt with the limit table 0 to 7) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_ADC_ERR0,GROUP3), /* ADC ERR0 (A/D converter unit 0 Error) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_ADC_ERR1,GROUP4), /* ADC ERR1 (A/D converter unit 1 Error) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_ADC_RESOVF0,GROUP5), /* ADC RESOVF0 (A/D conversion overflow on A/D converter unit 0) */
            [14] = BSP_PRV_VECT_ENUM(EVENT_ADC_RESOVF1,GROUP6), /* ADC RESOVF1 (A/D conversion overflow on A/D converter unit 1) */
            [15] = BSP_PRV_VECT_ENUM(EVENT_ADC_CALEND0,GROUP7), /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
            [16] = BSP_PRV_VECT_ENUM(EVENT_ADC_CALEND1,GROUP0), /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
            [17] = BSP_PRV_VECT_ENUM(EVENT_ADC_ADI0,GROUP1), /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
            [18] = BSP_PRV_VECT_ENUM(EVENT_ADC_ADI1,GROUP2), /* ADC ADI1 (End of A/D scanning operation(Gr.1)) */
            [19] = BSP_PRV_VECT_ENUM(EVENT_ADC_ADI2,GROUP3), /* ADC ADI2 (End of A/D scanning operation(Gr.2)) */
            [20] = BSP_PRV_VECT_ENUM(EVENT_ADC_ADI3,GROUP4), /* ADC ADI3 (End of A/D scanning operation(Gr.3)) */
            [21] = BSP_PRV_VECT_ENUM(EVENT_ADC_ADI4,GROUP5), /* ADC ADI4 (End of A/D scanning operation(Gr.4)) */
            [22] = BSP_PRV_VECT_ENUM(EVENT_ADC_FIFOOVF,GROUP6), /* ADC FIFOOVF (FIFO data overflow) */
            [23] = BSP_PRV_VECT_ENUM(EVENT_ADC_FIFOREQ0,GROUP7), /* ADC FIFOREQ0 (FIFO data read request interrupt(Gr.0)) */
            [24] = BSP_PRV_VECT_ENUM(EVENT_ADC_FIFOREQ1,GROUP0), /* ADC FIFOREQ1 (FIFO data read request interrupt(Gr.1)) */
            [25] = BSP_PRV_VECT_ENUM(EVENT_ADC_FIFOREQ2,GROUP1), /* ADC FIFOREQ2 (FIFO data read request interrupt(Gr.2)) */
            [26] = BSP_PRV_VECT_ENUM(EVENT_ADC_FIFOREQ3,GROUP2), /* ADC FIFOREQ3 (FIFO data read request interrupt(Gr.3)) */
            [27] = BSP_PRV_VECT_ENUM(EVENT_ADC_FIFOREQ4,GROUP3), /* ADC FIFOREQ4 (FIFO data read request interrupt(Gr.4)) */
            [28] = BSP_PRV_VECT_ENUM(EVENT_SCI1_RXI,GROUP4), /* SCI1 RXI (Receive data full) */
            [29] = BSP_PRV_VECT_ENUM(EVENT_SCI1_TXI,GROUP5), /* SCI1 TXI (Transmit data empty) */
            [30] = BSP_PRV_VECT_ENUM(EVENT_SCI1_TEI,GROUP6), /* SCI1 TEI (Transmit end) */
            [31] = BSP_PRV_VECT_ENUM(EVENT_SCI1_ERI,GROUP7), /* SCI1 ERI (Receive error) */
            [32] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TXI,FIXED), /* SCI4 TXI (Transmit data empty) */
            [33] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TEI,FIXED), /* SCI4 TEI (Transmit end) */
            [34] = BSP_PRV_VECT_ENUM(EVENT_SCI5_TXI,FIXED), /* SCI5 TXI (Transmit data empty) */
            [35] = BSP_PRV_VECT_ENUM(EVENT_SCI5_TEI,FIXED), /* SCI5 TEI (Transmit end) */
            [36] = BSP_PRV_VECT_ENUM(EVENT_SCI8_RXI,FIXED), /* SCI8 RXI (Receive data full) */
            [37] = BSP_PRV_VECT_ENUM(EVENT_SCI8_TXI,FIXED), /* SCI8 TXI (Transmit data empty) */
            [38] = BSP_PRV_VECT_ENUM(EVENT_SCI8_TEI,FIXED), /* SCI8 TEI (Transmit end) */
            [39] = BSP_PRV_VECT_ENUM(EVENT_SCI8_ERI,FIXED), /* SCI8 ERI (Receive error) */
        };
        #endif
        #endif