#include<string.h>
#include "uart.h"
#include "UART_DEFINES.h"
#include "lcd.h"
#include "delay.h"
/* Buffer to store received SMS message */
char received_msg[50];
/* Global variables updated by UART ISR */
extern char buffer[300],read_flag,ch;
extern unsigned char dummy;
extern int i,flag;
/*Initialize GSM module and configure SMS mode*/
void gsm_init()
{
	/* Wait for GSM module power-up */
  delay_ms(6000);
  memset(buffer,0,300);
  i=0;
  read_flag=0;
  /* Send basic AT command to check GSM communication */
  U0_TX_BYTE_string("AT\r\n");
	/* Display command on LCD */
  cmd_lcd(0x80);
  str_lcd("AT");
	/* Wait until response characters are received */
  while(i<2);
	/* Check if GSM replied with OK */
  if(strstr(buffer,"OK"))
  {
    //flash GSM response on lcd in second line
	  cmd_lcd(0xc0);		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 				                                                                                                                                                                                                                                                                                                                              		
	  str_lcd("OK");
	  delay_ms(1000);
  }
  else
  {
    //flash error on lcd in second line
	  cmd_lcd(0xc0);
	  str_lcd("Error");
	  delay_ms(1000);
  }
	/* Clear buffer before next command */
  memset(buffer,0,300);
  i=0;
  read_flag=0;
	/* Disable command echo */
  U0_TX_BYTE_string("ATE0\r\n");
	/* Display command on LCD */
  cmd_lcd(0x01);
  cmd_lcd(0x80);
  str_lcd("ATE0");
  /* Wait until response characters are received */	
  while(i<2);
	/* Check if GSM replied with OK */
  if(strstr(buffer,"OK"))
  {
    //flash GSM response on lcd in second line
	  cmd_lcd(0xc0);
	  str_lcd("OK");
	  delay_ms(1000);
  }
  else
  {
		//flash error on lcd in second line
    cmd_lcd(0xc0);
	  str_lcd(buffer);
	  delay_ms(1000);  
  }
	
  memset(buffer,0,300);
  i=0;
  read_flag=0;
	/* Configure GSM in text mode SMS */
  U0_TX_BYTE_string("AT+CMGF=1\r\n");	
	/* Display command on LCD */
	cmd_lcd(0x01);
  cmd_lcd(0x80);
  str_lcd("AT+CMGF=1");
	/* Wait until response characters are received */
  while(i<2);
  if(strstr(buffer,"OK"))
  {
   //flash GSM response on lcd in second line
	 cmd_lcd(0xc0);
	 str_lcd("OK");
	 delay_ms(1000);
  }
  else
  {
		//flash GSM response on lcd in second line
    cmd_lcd(0xc0);
	  str_lcd("ERROR");
	  delay_ms(1000);
  }
  memset(buffer,0,300);
  i=0;
  read_flag=0;
	/* Delete SMS stored at location 1 */
  U0_TX_BYTE_string("AT+CMGD=1\r\n");
	/* Display command on LCD */
  cmd_lcd(0x01);
  cmd_lcd(0x80);
  str_lcd("AT+CMGD=1");
  delay_ms(1000);
  if(strstr(buffer,"OK"))
	{
		//flash GSM response on lcd in second line
		cmd_lcd(0xc0);
	  str_lcd("OK");
		delay_ms(1000);
	}
	else
	{	
    //flash GSM response on lcd in second line		
		cmd_lcd(0xc0);
	  str_lcd("ERROR");
		delay_ms(1000);
	}
	memset(buffer,0,300);
	i=0;
	/*
     Configure GSM to immediately notify
     microcontroller whenever a new SMS arrives.
   */
  U0_TX_BYTE_string("AT+CNMI=2,1,0,0,0\r\n");
	memset(buffer,0,300);
	delay_ms(3000);
	/* Display command on LCD */
	cmd_lcd(0x01);
  cmd_lcd(0x80);
	str_lcd("CNMI");
	while(i<4);
	buffer[i]='\0';
	if(strstr(buffer,"OK"))
	{
		//flash GSM response on lcd in second line
		cmd_lcd(0xc0);
		str_lcd("OK");
		delay_ms(1000);
	}
 }
/*Send an SMS message through GSM module to a specified mobile number*/
void gsm_send(char *mobilenumber,char *msg)
{
	 /* Clear receive buffer */
   memset(buffer,0,300);
   i=0;
   read_flag=0;
	 /* Display command on LCD */
   cmd_lcd(0x80);
   str_lcd("AT+CMGS=");
   delay_ms(1000);
	 /* Send AT command for SMS sending */
   U0_TX_BYTE_string("AT+CMGS=");
   U0_TXBYTE('"');
	 /* Send destination mobile number */
   U0_TX_BYTE_string(mobilenumber);
   U0_TXBYTE('"');
   U0_TX_BYTE_string("\r\n");
   delay_ms(2000);
    i=0;
	  /* Send SMS text */
    U0_TX_BYTE_string(msg);
	  /* Send Ctrl+Z (0x1A) to terminate SMS */
	  U0_TXBYTE(0X1A);
	  delay_ms(3000);
		 /* Check GSM response */
	  if(strstr(buffer,"OK"))
	  {
		  cmd_lcd(0x01);
		  cmd_lcd(0x80);
		  str_lcd("SMS SENT");
		  delay_ms(1000);
	  }
	  else
	  {
		  cmd_lcd(0x01);
		  cmd_lcd(0x80);
		  str_lcd(buffer);
		  str_lcd("SMS NOT");
		  delay_ms(1000);
	  }
	  cmd_lcd(0X01);
}
/* Function to read received SMS from GSM module 
Extracts only the SMS text from the GSM response*/
char* gsm_receive()
{
	 /* Pointer used for tokenizing GSM response */
   char *ptr;
	 /* Clear SMS message buffer */
   memset(received_msg,0,100);
	 /* Clear GSM response buffer */
   memset(buffer,0,300);
   i=0;
   read_flag=0;
   /* Read SMS stored in location 1 */
   U0_TX_BYTE_string("AT+CMGR=1\r\n");
   delay_ms(3000);
	 /* Display command on LCD */
	 cmd_lcd(0x01);
	 cmd_lcd(0x80);
   str_lcd("AT+CMGR=1");
	 delay_ms(3000);
	 buffer[i]='\0';
	 /* Check whether GSM returned SMS data */
	 if(strstr(buffer,"+CMGR")!=NULL)
	 {
		  /* Split response line-by-line */
	   ptr=strtok(buffer,"\r\n");
		 /*cmd_lcd(0x01);
		 str_lcd("READ");
		 delay_ms(2000);*/
	   while(ptr!=NULL)
	   {
		   /*cmd_lcd(0x01);
		   cmd_lcd(0x80); 
		   str_lcd("Enter");*/
			 /*Extracting the received message*/
	     if((strstr(ptr,"+CMGR")==NULL))
			 {
					strcpy(received_msg,ptr);
					break;
			 }
				  ptr=strtok(NULL,"\r\n");
	    }
		 /* Display SMS received message */
		 cmd_lcd(0x01);
	   cmd_lcd(0x80);
		 str_lcd("MSG RECEIVED");
		 delay_ms(1000);
	   cmd_lcd(0xC0);
	   str_lcd(received_msg);
	   delay_ms(3000);
	 }
	 else
	 {
	   cmd_lcd(0x01);
	   cmd_lcd(0x80);
	   str_lcd("NOT RECEIVED");
	   delay_ms(1000);
	 }
	   i=0;
     read_flag=0;
	   /*Delete SMS from location 1*/
     U0_TX_BYTE_string("AT+CMGD=1\r\n");                                                                                                                         
	   delay_ms(2000);
	    /*Display command on LCD*/
	   cmd_lcd(0x01);
	   cmd_lcd(0x80);
	   str_lcd("AT+CMGD=1");
	   delay_ms(1000);
	   /* Check delete status */
     if(strstr(buffer,"OK"))
		 {
			 cmd_lcd(0xc0);
	     str_lcd("OK");
		 } 
		 else
		 {				   
		   cmd_lcd(0xc0);
	     str_lcd("ERROR");
		 }
		 /* Return extracted SMS text */ 
		return received_msg;
}
    
