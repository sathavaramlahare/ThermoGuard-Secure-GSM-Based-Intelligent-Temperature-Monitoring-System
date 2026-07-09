#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define ADCCLK 3000000 //ADC clk frequency is lessthan 4.5MHZ
#define CLK_DIV ((PCLK/ADCCLK)-1)
#define PDN (1<<21) //Powerdown bit 21
#define CLKDIV_VALUE (CLK_DIV<<8)
#define START_CONV (1<<24)
#define DONE_BIT 31
#define Result 6

#define CH0 0x01
#define CH1 0x02
#define CH2 0x04
#define CH3 0x08

#define AIN0 0x00400000
#define AIN1 0x01000000
#define AIN2 0x04000000
#define AIN3 0x10000000
//ADC FUNCTION DECLARATIONS 
void INIT_ADC(void);
void Read_ADC(char chno,int *Adcval,float *eAR);
void display_temp(void);
