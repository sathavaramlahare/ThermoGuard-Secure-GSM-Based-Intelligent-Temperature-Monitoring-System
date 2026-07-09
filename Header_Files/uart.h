void INIT_UART(void);
void U0_TXBYTE(char SBYTE);
void U0_TX_BYTE_string(char *SBYTE);
void U0_TX_BYTE_int(int SBYTE);
char U0_RX_BYTE(void);
void uart0_isr(void) __irq;
void enable_uart0(void);
