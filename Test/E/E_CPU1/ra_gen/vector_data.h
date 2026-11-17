/* generated vector header file - do not edit */
        #ifndef VECTOR_DATA_H
        #define VECTOR_DATA_H
        #ifdef __cplusplus
        extern "C" {
        #endif
                /* Number of interrupts allocated */
        #ifndef VECTOR_DATA_IRQ_COUNT
        #define VECTOR_DATA_IRQ_COUNT    (40)
        #endif
        /* ISR prototypes */
        void ipc_isr(void);
        void canfd_error_isr(void);
        void canfd_channel_tx_isr(void);
        void canfd_common_fifo_rx_isr(void);
        void canfd_rx_fifo_isr(void);
        void sci_b_uart_rxi_isr(void);
        void sci_b_uart_txi_isr(void);
        void sci_b_uart_tei_isr(void);
        void sci_b_uart_eri_isr(void);
        void adc_b_limclpi_isr(void);
        void adc_b_err0_isr(void);
        void adc_b_err1_isr(void);
        void adc_b_resovf0_isr(void);
        void adc_b_resovf1_isr(void);
        void adc_b_calend0_isr(void);
        void adc_b_calend1_isr(void);
        void adc_b_adi0_isr(void);
        void adc_b_adi1_isr(void);
        void adc_b_adi2_isr(void);
        void adc_b_adi3_isr(void);
        void adc_b_adi4_isr(void);
        void adc_b_fifoovf_isr(void);
        void adc_b_fiforeq0_isr(void);
        void adc_b_fiforeq1_isr(void);
        void adc_b_fiforeq2_isr(void);
        void adc_b_fiforeq3_isr(void);
        void adc_b_fiforeq4_isr(void);
        void sci_b_i2c_txi_isr(void);
        void sci_b_i2c_tei_isr(void);
        void sci_b_spi_rxi_isr(void);
        void sci_b_spi_txi_isr(void);
        void sci_b_spi_tei_isr(void);
        void sci_b_spi_eri_isr(void);

        /* Vector table allocations */
        #define VECTOR_NUMBER_IPC_IRQ0 ((IRQn_Type) 0) /* IPC IRQ0 (CPU Mutual Interrupt 0) */
        #define IPC_IRQ0_IRQn          ((IRQn_Type) 0) /* IPC IRQ0 (CPU Mutual Interrupt 0) */
        #define VECTOR_NUMBER_CAN1_CHERR ((IRQn_Type) 1) /* CAN1 CHERR (Channel  error) */
        #define CAN1_CHERR_IRQn          ((IRQn_Type) 1) /* CAN1 CHERR (Channel  error) */
        #define VECTOR_NUMBER_CAN1_TX ((IRQn_Type) 2) /* CAN1 TX (Transmit interrupt) */
        #define CAN1_TX_IRQn          ((IRQn_Type) 2) /* CAN1 TX (Transmit interrupt) */
        #define VECTOR_NUMBER_CAN1_COMFRX ((IRQn_Type) 3) /* CAN1 COMFRX (Common FIFO receive interrupt) */
        #define CAN1_COMFRX_IRQn          ((IRQn_Type) 3) /* CAN1 COMFRX (Common FIFO receive interrupt) */
        #define VECTOR_NUMBER_CAN_GLERR ((IRQn_Type) 4) /* CAN GLERR (Global error) */
        #define CAN_GLERR_IRQn          ((IRQn_Type) 4) /* CAN GLERR (Global error) */
        #define VECTOR_NUMBER_CAN_RXF ((IRQn_Type) 5) /* CAN RXF (Global receive FIFO interrupt) */
        #define CAN_RXF_IRQn          ((IRQn_Type) 5) /* CAN RXF (Global receive FIFO interrupt) */
        #define VECTOR_NUMBER_SCI0_RXI ((IRQn_Type) 6) /* SCI0 RXI (Receive data full) */
        #define SCI0_RXI_IRQn          ((IRQn_Type) 6) /* SCI0 RXI (Receive data full) */
        #define VECTOR_NUMBER_SCI0_TXI ((IRQn_Type) 7) /* SCI0 TXI (Transmit data empty) */
        #define SCI0_TXI_IRQn          ((IRQn_Type) 7) /* SCI0 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI0_TEI ((IRQn_Type) 8) /* SCI0 TEI (Transmit end) */
        #define SCI0_TEI_IRQn          ((IRQn_Type) 8) /* SCI0 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI0_ERI ((IRQn_Type) 9) /* SCI0 ERI (Receive error) */
        #define SCI0_ERI_IRQn          ((IRQn_Type) 9) /* SCI0 ERI (Receive error) */
        #define VECTOR_NUMBER_ADC_LIMCLPI ((IRQn_Type) 10) /* ADC LIMCLPI (Limiter clip interrupt with the limit table 0 to 7) */
        #define ADC_LIMCLPI_IRQn          ((IRQn_Type) 10) /* ADC LIMCLPI (Limiter clip interrupt with the limit table 0 to 7) */
        #define VECTOR_NUMBER_ADC_ERR0 ((IRQn_Type) 11) /* ADC ERR0 (A/D converter unit 0 Error) */
        #define ADC_ERR0_IRQn          ((IRQn_Type) 11) /* ADC ERR0 (A/D converter unit 0 Error) */
        #define VECTOR_NUMBER_ADC_ERR1 ((IRQn_Type) 12) /* ADC ERR1 (A/D converter unit 1 Error) */
        #define ADC_ERR1_IRQn          ((IRQn_Type) 12) /* ADC ERR1 (A/D converter unit 1 Error) */
        #define VECTOR_NUMBER_ADC_RESOVF0 ((IRQn_Type) 13) /* ADC RESOVF0 (A/D conversion overflow on A/D converter unit 0) */
        #define ADC_RESOVF0_IRQn          ((IRQn_Type) 13) /* ADC RESOVF0 (A/D conversion overflow on A/D converter unit 0) */
        #define VECTOR_NUMBER_ADC_RESOVF1 ((IRQn_Type) 14) /* ADC RESOVF1 (A/D conversion overflow on A/D converter unit 1) */
        #define ADC_RESOVF1_IRQn          ((IRQn_Type) 14) /* ADC RESOVF1 (A/D conversion overflow on A/D converter unit 1) */
        #define VECTOR_NUMBER_ADC_CALEND0 ((IRQn_Type) 15) /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
        #define ADC_CALEND0_IRQn          ((IRQn_Type) 15) /* ADC CALEND0 (End of calibration of A/D converter unit 0) */
        #define VECTOR_NUMBER_ADC_CALEND1 ((IRQn_Type) 16) /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
        #define ADC_CALEND1_IRQn          ((IRQn_Type) 16) /* ADC CALEND1 (End of calibration of A/D converter unit 1) */
        #define VECTOR_NUMBER_ADC_ADI0 ((IRQn_Type) 17) /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
        #define ADC_ADI0_IRQn          ((IRQn_Type) 17) /* ADC ADI0 (End of A/D scanning operation(Gr.0)) */
        #define VECTOR_NUMBER_ADC_ADI1 ((IRQn_Type) 18) /* ADC ADI1 (End of A/D scanning operation(Gr.1)) */
        #define ADC_ADI1_IRQn          ((IRQn_Type) 18) /* ADC ADI1 (End of A/D scanning operation(Gr.1)) */
        #define VECTOR_NUMBER_ADC_ADI2 ((IRQn_Type) 19) /* ADC ADI2 (End of A/D scanning operation(Gr.2)) */
        #define ADC_ADI2_IRQn          ((IRQn_Type) 19) /* ADC ADI2 (End of A/D scanning operation(Gr.2)) */
        #define VECTOR_NUMBER_ADC_ADI3 ((IRQn_Type) 20) /* ADC ADI3 (End of A/D scanning operation(Gr.3)) */
        #define ADC_ADI3_IRQn          ((IRQn_Type) 20) /* ADC ADI3 (End of A/D scanning operation(Gr.3)) */
        #define VECTOR_NUMBER_ADC_ADI4 ((IRQn_Type) 21) /* ADC ADI4 (End of A/D scanning operation(Gr.4)) */
        #define ADC_ADI4_IRQn          ((IRQn_Type) 21) /* ADC ADI4 (End of A/D scanning operation(Gr.4)) */
        #define VECTOR_NUMBER_ADC_FIFOOVF ((IRQn_Type) 22) /* ADC FIFOOVF (FIFO data overflow) */
        #define ADC_FIFOOVF_IRQn          ((IRQn_Type) 22) /* ADC FIFOOVF (FIFO data overflow) */
        #define VECTOR_NUMBER_ADC_FIFOREQ0 ((IRQn_Type) 23) /* ADC FIFOREQ0 (FIFO data read request interrupt(Gr.0)) */
        #define ADC_FIFOREQ0_IRQn          ((IRQn_Type) 23) /* ADC FIFOREQ0 (FIFO data read request interrupt(Gr.0)) */
        #define VECTOR_NUMBER_ADC_FIFOREQ1 ((IRQn_Type) 24) /* ADC FIFOREQ1 (FIFO data read request interrupt(Gr.1)) */
        #define ADC_FIFOREQ1_IRQn          ((IRQn_Type) 24) /* ADC FIFOREQ1 (FIFO data read request interrupt(Gr.1)) */
        #define VECTOR_NUMBER_ADC_FIFOREQ2 ((IRQn_Type) 25) /* ADC FIFOREQ2 (FIFO data read request interrupt(Gr.2)) */
        #define ADC_FIFOREQ2_IRQn          ((IRQn_Type) 25) /* ADC FIFOREQ2 (FIFO data read request interrupt(Gr.2)) */
        #define VECTOR_NUMBER_ADC_FIFOREQ3 ((IRQn_Type) 26) /* ADC FIFOREQ3 (FIFO data read request interrupt(Gr.3)) */
        #define ADC_FIFOREQ3_IRQn          ((IRQn_Type) 26) /* ADC FIFOREQ3 (FIFO data read request interrupt(Gr.3)) */
        #define VECTOR_NUMBER_ADC_FIFOREQ4 ((IRQn_Type) 27) /* ADC FIFOREQ4 (FIFO data read request interrupt(Gr.4)) */
        #define ADC_FIFOREQ4_IRQn          ((IRQn_Type) 27) /* ADC FIFOREQ4 (FIFO data read request interrupt(Gr.4)) */
        #define VECTOR_NUMBER_SCI1_RXI ((IRQn_Type) 28) /* SCI1 RXI (Receive data full) */
        #define SCI1_RXI_IRQn          ((IRQn_Type) 28) /* SCI1 RXI (Receive data full) */
        #define VECTOR_NUMBER_SCI1_TXI ((IRQn_Type) 29) /* SCI1 TXI (Transmit data empty) */
        #define SCI1_TXI_IRQn          ((IRQn_Type) 29) /* SCI1 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI1_TEI ((IRQn_Type) 30) /* SCI1 TEI (Transmit end) */
        #define SCI1_TEI_IRQn          ((IRQn_Type) 30) /* SCI1 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI1_ERI ((IRQn_Type) 31) /* SCI1 ERI (Receive error) */
        #define SCI1_ERI_IRQn          ((IRQn_Type) 31) /* SCI1 ERI (Receive error) */
        #define VECTOR_NUMBER_SCI4_TXI ((IRQn_Type) 32) /* SCI4 TXI (Transmit data empty) */
        #define SCI4_TXI_IRQn          ((IRQn_Type) 32) /* SCI4 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI4_TEI ((IRQn_Type) 33) /* SCI4 TEI (Transmit end) */
        #define SCI4_TEI_IRQn          ((IRQn_Type) 33) /* SCI4 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI5_TXI ((IRQn_Type) 34) /* SCI5 TXI (Transmit data empty) */
        #define SCI5_TXI_IRQn          ((IRQn_Type) 34) /* SCI5 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI5_TEI ((IRQn_Type) 35) /* SCI5 TEI (Transmit end) */
        #define SCI5_TEI_IRQn          ((IRQn_Type) 35) /* SCI5 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI8_RXI ((IRQn_Type) 36) /* SCI8 RXI (Receive data full) */
        #define SCI8_RXI_IRQn          ((IRQn_Type) 36) /* SCI8 RXI (Receive data full) */
        #define VECTOR_NUMBER_SCI8_TXI ((IRQn_Type) 37) /* SCI8 TXI (Transmit data empty) */
        #define SCI8_TXI_IRQn          ((IRQn_Type) 37) /* SCI8 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI8_TEI ((IRQn_Type) 38) /* SCI8 TEI (Transmit end) */
        #define SCI8_TEI_IRQn          ((IRQn_Type) 38) /* SCI8 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI8_ERI ((IRQn_Type) 39) /* SCI8 ERI (Receive error) */
        #define SCI8_ERI_IRQn          ((IRQn_Type) 39) /* SCI8 ERI (Receive error) */
        /* The number of entries required for the ICU vector table. */
        #define BSP_ICU_VECTOR_NUM_ENTRIES (40)

        #ifdef __cplusplus
        }
        #endif
        #endif /* VECTOR_DATA_H */