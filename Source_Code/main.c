#include "gsm.h"
#include "uart.h"
#include "UART_DEFINES.h"
#include "lcd.h"
#include "keypad.h"
#include "interrupt.h"
#include "adc.h"
#include "main_defines.h"
#include "delay.h"
#include "local_settings.h"
#include "i2c.h"
#include <string.h>
//set point of the temperature
int setpoint=200;
int temp,temp_alert_sent;
/* Flags shared with interrupt routines */
extern volatile int eint_flag;
extern int received_flag,read_flag,i;
/* Authorized mobile number */
char mobilenumber[11]="9573641321";
/* Temporary buffer for number extraction */
char new_buffer[20];
/* Variables defined in GSM source file */
extern char received_msg[50],buffer[300];
/* Password stored in EEPROM */
extern char passkey[];
char *message;
/* EEPROM initialization flag */
char eflag[7]="1107@s";
/* Temporary EEPROM data buffer */
char tem[7];
/*Initializes all peripherals and loads saved settings
   from EEPROM.*/
void INIT()
{
	// Initialize LCD
  LCD_INIT();
	// Initialize UART
  INIT_UART();
	// Initialize I2C
  init_i2c();
	// Enable UART interrupt
  enable_uart0();
	// Initialize ADC
  INIT_ADC();
	// Initialize keypad
  KPM_INIT();
	// Enable external interrupt
  enable_eint0();
	// Initialize GSM module
  gsm_init();
	//Read EEPROM Initialization Flag
  memset(tem,0,sizeof(tem));
	/* Read EEPROM flag from address 0x40 */
  i2c_seq_read(0x50,0x40,tem,6);
  tem[6]='\0';
	/*EEPROM First-Time Initialization
	Stores defaults only once.*/
  if(strcmp(tem,eflag)!=0)
  {
  i2c_byte_write(0x50,0x01,setpoint);
  i2c_page_write(0x50,0x02,mobilenumber,10);
  i2c_page_write(0x50,0x20,passkey,4);
  //i2c_page_write(0x50,0x40,1107);
  i2c_page_write(0x50,0x40,eflag,7);
  }
	//Load saved settings from EEPROM
  setpoint = i2c_random_read(0x50,0x01);
  i2c_seq_read(0x50,0x02,mobilenumber,10);
  i2c_seq_read(0x50,0x20,passkey,4);
  mobilenumber[10] = '\0';                   
}
int main()
{
	//ADC Variables
  int dval;
  float ear;
	/*Initialize all peripherals */
  INIT();
  while(1)
  {
		/*Check external interrupt*/
    if(eint_flag==1)
		{
			 eint_flag^=1;
		   check_password();
		 }
    /* Read temperature sensor*/		
		Read_ADC(CH1,&dval,&ear);
		/*Convert ADC voltage into temperature */
		temp=ear*100;
		/*Temperature Alert*/
		if(temp>setpoint)
		{
			/*Send SMS only once */
			if(temp_alert_sent==0)
			{
         gsm_send(mobilenumber,"Temperature exceeded");
         temp_alert_sent=1;
			}				
    }

		cmd_lcd(0x01);
		cmd_lcd(0x80);
		str_lcd("Thermo Guard");
		delay_ms(1000);
		/*checks wheather sms received or not*/
		if(received_flag==1)
    {
		  cmd_lcd(0x01);
		  cmd_lcd(0x80);
		  str_lcd("FLAG");
		  delay_ms(1000);
			/*Clear receive flag */
		  received_flag=0;
			/* Read SMS */
		  message=gsm_receive();
			/* If message received successfully */
		  if(message!=0)
		  {
		     /*cmd_lcd(0x01);
			   cmd_lcd(0x80);
			   str_lcd(message);
		     delay_ms(3000);*/
				 /* Verify sender mobile number*/
			   if(verify_number(buffer)==0)
			   {
					  /*Verify SMS syntax*/
			      if(check_syntax(message))
			      {
						  cmd_lcd(0x80);
						  str_lcd("valid");
						  cmd_lcd(0xc0);
						  read(message);
				    }
				  else
				  {
						  cmd_lcd(0x01);
						  cmd_lcd(0x80);
						  str_lcd("Invalid Message");
						  delay_ms(1000);
				  }
			   }
			   else
			   {
	          /*Unauthorized Number*/  				 
			     	cmd_lcd(0x01);
				    cmd_lcd(0x80);
				    str_lcd("unauthorised");
					  delay_ms(1000);
					  /*Sends sms to authorized number*/
				    gsm_send(mobilenumber,"unauthorized access");
			    }
       }	 
     }
  }
}
/*Extracts sender mobile number from GSM response
  and compares it with authorized number.*/
int verify_number(char *buffer)
{
  int i=0,cnt=0;
  while(buffer[i]!='9')i++;
  i=i+2;
	/*Copy 10 digits into temporary buffer*/
  while(cnt<10)
  {
    new_buffer[cnt++]=buffer[i++];
  }
  new_buffer[cnt]='\0';
	/*Compare extracted number with stored number*/
  if(strcmp(new_buffer,mobilenumber)==0)
     return 0;// Authorized
  return 1;// unauthorized
}
