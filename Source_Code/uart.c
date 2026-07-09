#include<lpc21xx.h>
#include"UART_DEFINES.h"
#include<string.h>
#include"uart.h"
#include"lcd.h"
char buffer[300],read_flag;
volatile int i=0,received_flag=0;
unsigned char ch,dummy;
/*Initializes UART0 for communication with the GSM module.
 Configures:
    - TX/RX pins
    - Baud rate
    - Frame format (8N1)
    - UART interrupts*/
void INIT_UART()
{
	/*clear previous pin settings*/
	PINSEL0&=~15;
	/*Configure P0.0 as TXD0 and P0.1 as RXD0*/
	PINSEL0|=TXD0_PIN_EN|RXD0_PIN_EN;
	/*Enable access to baud-rate registers (DLAB=1)*/
	/*Cfg UART frame for 8N1(8bits,0 parity,1 Stopbit)*/
	U0LCR=(1<<DLAB_BIT)|WORD_LEN_SEL_BITS;
	
	/*Load baud-rate divisor (Lower Byte)*/
	U0DLL= DIVISOR;
	/*Load baud-rate divisor (Higher Byte)*/
	U0DLM= DIVISOR>>8;
	
	/*Disable DLAB after baud rate configuration*/
	U0LCR&=~(1<<DLAB_BIT);
	
	/*Enable UART Receive Interrupt*/ 
	U0IER= (1<<0);
	cmd_lcd(0x80);
}
/*Transmits a single byte through UART0.*/
void U0_TXBYTE(char SBYTE)
{
	/*Load byte into UART transmit holding register*/
	U0THR=SBYTE;
	/*wait until TX complete status*/
	while(((U0LSR>>TEMT_BIT)&1)==0);
}
/* Sends an entire string through UART.*/
void U0_TX_BYTE_string(char *SBYTE)
{
	while(*SBYTE)
	{
		U0_TXBYTE(*SBYTE++);
	}
	//wait until TX complete status
	while(((U0LSR>>TEMT_BIT)&1)==0);
}
/*Receives one byte using polling.*/
char U0_RX_BYTE()
{
	//wait until byte received and buffered
	while(((U0LSR>>DR)&1)==0);
	//return data stored in buffer
	return U0RBR;
}
/*UART0 Interrupt Service Routine.

 Handles:
   - Receiving characters
   - Building received GSM response
   - Detecting new SMS notification (+CMTI)*/
void uart0_isr(void) __irq			 
{
	/*Check whether interrupt is Receive Data Available*/
	if((U0IIR & 0X0E)== 0X04)
	{
      /*Read received character*/
	    ch=U0RBR;
		/*Store received character into buffer*/
		if(ch!='\n'|| ch!='\r')
		{
	    buffer[i]=ch;
	    i++;
		}
		/*Check whether GSM has notified a new SMS*/
		if(strstr(buffer,"+CMTI")!=NULL)
		{	
      /*Inform main program that SMS has arrived*/		
		  received_flag=1;
		}	
		if(ch=='\n'|| ch=='\r')
		{
		   buffer[i]='\0';
		   read_flag=1;
		} 
    }
	else
	{
		/*Dummy read to clear unwanted interrupt source*/
	  dummy=U0IIR;
	}
	/*Inform VIC that ISR execution is complete*/
	VICVectAddr=0;
}
/*Enables UART0 interrupt in the VIC*/
void enable_uart0()
{
	/*Configure UART interrupt as IRQ*/
	VICIntSelect=0x00000000;
	/*Load UART ISR address into vector slot 0*/
	VICVectAddr0=(unsigned int)uart0_isr;
	/*Enable vector slot 0 and assign UART0 (Channel 6)*/
	VICVectCntl0=(1<<5)| 6;
	/*Enable UART0 interrupt in VIC*/
	VICIntEnable=(1<<6);
}

