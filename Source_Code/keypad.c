#include<lpc21xx.h>
#include"lcd.h"
#include"keypad.h"
//Lookup table used for row and column scanning
int valLUT[4]={14,13,11,7};
//Keypad layout mapping
 char KPMLUT[4][4]={{'7','8','9','.'},
                   {'4','5','6','@'},
                   {'1','2','3','#'},
                   {'+','0','*','-'}};

//Initializes keypad rows as output pins
void KPM_INIT()
{
  // Configure row pins as output
  IODIR1|=0XF<<ROW0;
}

//Checks whether any key is pressed
int colscan(void)
{
	  // Read column pins and check for key press
     if(((IOPIN1>>COL0)&15)<15)
     {
        return 1; //switch is pressed
     }
     else
     {
        return 0;//switch is not pressed
     }
}

//Identifies which row contains the pressed key
int rowcheck(void)
{

  int  rowno;
   // Scan all rows one by one
  for(rowno=0;rowno<=3;rowno++)
  {
		// Activate one row at a time
    IOPIN1=((IOPIN1&~(15<<ROW0))|(valLUT[rowno]<<ROW0));
		// Check whether key is pressed in current row
    if(colscan()==1)
    {
       break;
     }
   }
   // Clear all rows after scanning
   IOPIN1=((IOPIN1&~(15<<ROW0))|(0X0<<ROW0));
   return rowno;
}

//Identifies which column contains the pressed key
int colcheck(void)
{
  int colno;
	// Scan all columns
  for(colno=0;colno<=3;colno++)
  {
	// Compare current column value with lookup table
    if(((IOPIN1>>COL0)&15)==valLUT[colno])
       break;
  }
  return colno;
}
//Detects pressed key and returns corresponding value
int keyscan(void)
{
  int rowno,colno,keyval;
  //Wait until any switch is pressed
  while(colscan()==0);
	// Find row number of pressed key
	rowno=rowcheck();
	//Find column number of pressed key
	colno=colcheck();
  // Get corresponding key value from lookup table
	keyval=KPMLUT[rowno][colno];
	//wait for switch release
	while(colscan());
	return keyval;
}
