#include<lpc21xx.h>
#define FOSC 12000000
#define CCLK FOSC*5
#define PCLK CCLK/4
#define BAUD 9600
#define DIVISOR (PCLK/(16*BAUD))

//defines of U0LCR
#define _8bits 3
#define WORD_LEN_SEL_BITS _8bits
#define DLAB_BIT 7

//defines for U0LSR
#define DR 0
#define TEMT_BIT 6

//defines of TX,RX Pin in PINSEL0 SFR
#define TXD0_PIN_EN 0x00000001
#define RXD0_PIN_EN 0x00000004
