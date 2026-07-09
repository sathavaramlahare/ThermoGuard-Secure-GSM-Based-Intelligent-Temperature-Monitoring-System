#include"delay.h"
#include"lcd.h"
#include"keypad.h"
#include<string.h>
#include<stdlib.h>
#include"i2c.h"
#include"gsm.h"
void local_setpoint(void);
/*Current temperature setpoint stored in main.c */
extern int setpoint;
/* Buffer storing received GSM message */
extern char received_msg[50];
/* Flag indicating whether setpoint has been entered */
int set_point_flag=0;
extern char mobilenumber[11];
/* Local password for keypad authentication */
char password[5]="2222";
/* Array to store user-entered password */
char input[5];
/* Buffer used for sending setpoint through GSM */
char set[11];
/*Authenticates the user using the keypad.*/
int check_password(void)
{
   int i=0;
   char key;
	 i=0;
		   // Clear LCD and display password prompt
	       cmd_lcd(0x01);
		   cmd_lcd(0x80);
		   str_lcd("Enter the pin:");
		   cmd_lcd(0xc0);
		 	  /* Read password continuously */
		   while(1)
		  {
				 /*Scan keypad key*/
			   key=keyscan();
			   if(key=='\0')
				    continue;
				 /*Backspace operation*/
				if(key=='#')
			{
				if(i>0)
				{
					i--;
					cmd_lcd(0x10);
					char_lcd(' ');
					cmd_lcd(0x10);
				}
			}
			/*Accept only numeric digits*/
			else if(key>='0' && key<='9')
			{
				/*Maximum password length is 4 digits*/
				if(i<4)
				{
					 /* Store entered digit */
			     input[i]=key;
			     char_lcd(key);
			     delay_ms(100);
					 // password hiding
			     cmd_lcd(0x10);
			     char_lcd('*');
			     i++;
				}
			 }
			/*'*' key used as ENTER key*/
			else if(key == '*')   
      {
				/* Accept only if exactly 4 digits entered */
			  if(i==4)
			  {
           input[i] = '\0';
           break;
			  }				
     }
	  }
		    input[i]='\0';
		    cmd_lcd(0x01);
		    delay_ms(100);
		    /*Compare entered password with stored password*/
		    if(strcmp(input,password)==0)
		    {
            /*Open local settings menu*/
			     local_setpoint();
					      return 1;
		    }
		    else
		    {      
            /*Wrong password message*/				
			      str_lcd("Access Denied");
			      delay_ms(1000);
				    cmd_lcd(0X01);
					  /* Send unauthorized access SMS */
					  gsm_send(mobilenumber,"unauthorized local access attempted");
				    return 0;
		    }
}
/*Allows the user to update the temperature setpoint through the keypad after successful authentication.
   The new setpoint is:
 • Stored in EEPROM
 • Confirmation SMS is sent*/
void local_setpoint()
{
	char key;
	int i=0; 
	char new_setpoint[10];
	cmd_lcd(0x01);
	cmd_lcd(0x80);
	str_lcd("Enter setpoint");
	cmd_lcd(0xC0);
	/* Read keypad continuously */
	while(1)
	{
		key=keyscan();
		if(key=='\0')
		  continue;
		/* '#' acts as BACKSPACE*/
		 if(key=='#')
			{
				if(i>0)
				{
					i--;
					cmd_lcd(0x10);
					char_lcd(' ');
					cmd_lcd(0x10);
				}
			}
		/* Accept numeric digits only */
		if(key>='0' && key<='9')
		{
			new_setpoint[i++]=key;
			char_lcd(key);
			set_point_flag=1;
		}
		/* '*' acts as ENTER */
		else if(key=='*')
		{
		    new_setpoint[i]='\0';
			break;
		}
	}
   /* If user entered a valid setpoint */
	if(set_point_flag==1)
	{
		/*clear flag*/ 
	  set_point_flag=0;
	  /* Convert string to integer and store in EEPROM */
	  i2c_byte_write(0x50,0x01,atoi(new_setpoint));
	  delay_ms(10);
		 /* Notify user through SMS */
	  gsm_send(mobilenumber,"Setpoint updated");
	  delay_ms(1000);
		/* Send updated setpoint */
	  gsm_send(mobilenumber,new_setpoint);
		 /* Display confirmation */
	  cmd_lcd(0x01);
	  cmd_lcd(0x80);
	  str_lcd("SMS SENT");
	  delay_ms(1000);
	  set_point_flag=0;
	  cmd_lcd(0x01);
	}
}

