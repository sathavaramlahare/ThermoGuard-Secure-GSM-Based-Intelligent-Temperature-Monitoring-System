#include<lpc21xx.h>
#include"lcd.h"
#include"delay.h"
/*Register Select pin connected to P0.5*/
#define RS 5
/*Read/Write pin connected to P0.4*/
#define RW 4
/*Enable pin connected to p0.6*/
#define EN   6  
/*LCD Data lines D0-D7 connected from P0.8 onwards*/
#define DATA 8
/*global variables*/
int data;
char str[100];
/*Writes either command or data to LCD depending on the RS pin status.*/
void write_lcd(int data)
{
	/*Set RW = 0 (Write operation)*/
	IOCLR0=1<<RW;
	/*Clear previous data bits (P0.8-P0.15)
    Load new 8-bit data onto LCD data pins*/
	IOPIN0=(IOPIN0&~(0XFF<<DATA))|(data<<DATA);
  /*Generate Enable(High -> Low)*/
	IOSET0=1<<EN;
  /*Small delay for LCD setup time*/
	delay_us(10);
  /*Bring Enable Low so LCD latches the data*/
	IOCLR0=1<<EN;
	delay_ms(2);
}
/*Sends a command to LCD instruction register.*/
void cmd_lcd(int data)
{
	/*RS = 0 selects Command Register*/
	IOCLR0=1<<RS;
  /*Send command*/
	write_lcd(data);
}
/*Sends one character to LCD display.*/
void char_lcd(int data)
{
  /* RS = 1 selects Data Register*/
	IOSET0=1<<RS;
  /*Send character*/
	write_lcd(data);
}
/*Initializes LCD in 8-bit mode.*/
void LCD_INIT()
{
  /*Configure RS pin as Output*/
	IODIR0|=1<<RS;
  /*Configure RW pin as Output*/
	IODIR0|=1<<RW;
  /*Configure EN pin as Output*/
	IODIR0|=1<<EN;
  /*Configure Data Pins (P0.8-P0.15) as Output*/
	IODIR0|=0XFF<<DATA;
  /*Wait for LCD power stabilization*/
	delay_ms(15);
         //----------------------	
	    //LCD Initialization Sequence
         //----------------------
	/*Function Set (8-bit interface)*/
	cmd_lcd(0X30);
	delay_ms(5);
  /*Repeat Function Set*/
	cmd_lcd(0X30);
	delay_ms(10);
	/*Repeat Function Set*/
	cmd_lcd(0x30);
	      //----------------------
            //Configure LCD
        //----------------------
  /*8-bit mode, 2-line display*/
	cmd_lcd(0x38);
  /*Display ON, Cursor ON, Cursor Blink ON*/
	cmd_lcd(0x0F);
  /*clear LCD Screen*/
	cmd_lcd(0x01);
  /*Cursor moves right after each character*/
	cmd_lcd(0x06);
	/*Display ON, Cursor OFF, Blink OFF*/
	cmd_lcd(0x0c);
}
/* Displays a string on LCD.*/
void str_lcd(char *str)
{
  /*Display characters until NULL character*/
	while(*str)
	{
    /* Display current character and move pointer*/
		char_lcd(*str++);
	}
}
/*Displays an integer number on LCD.*/
void int_LCD(int n)
{
  int a[10],i=0;
  if(n==0)
  {
    char_lcd('0');
  }
  else
  {
		/*Separate digits one by one*/
  while(n)
  {
		/*Convert digit into ASCII*/
    a[i++]=(n%10)+48;
    n=n/10;
   }
	/*Print digits in reverse order*/
   for(--i;i>=0;i--)
   {
     char_lcd(a[i]);
   }
   }
 }

