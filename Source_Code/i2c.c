#include<lpc21xx.h>
/*SCL and SDA pin definitions*/
/*P0.2 configured as SCL*/
#define SCL_PIN 0x00000010 
/*P0.3 configured as SDA*/
#define SDA_PIN 0x00000040 

/*Processor clock frequency*/
#define CCLK 60000000
/*Peripheral clock frequency*/
#define PCLK (CCLK/4)
/*Required I2C bus speed (100 kHz)*/
#define I2C_SPEED 100000
/*Load value for SCL High and Low registers*/
#define LOADVAL ((PCLK/I2C_SPEED)/2)

/*Defines for CONSET*/
/* Assert acknowledge bit*/
#define AA_BIT 2
/*Interrupt flag*/
#define SI_BIT 3
/*STOP condition bit*/
#define STO_BIT 4
/*START condition bit*/
#define STA_BIT 5
/*I2C Enable bit*/
#define I2EN_BIT 6

/*Defines for CONCLR*/
/*Clear acknowledge*/
#define AAC_BIT 2
/*Clear interrupt flag*/
#define SIC_BIT 3
/*Clear START condition*/
#define STAC_BIT 5
/*Disable I2C*/
#define I2ENC_BIT 6

/* Initializes I2C peripheral*/
void init_i2c(void)
{
	/* Configure P0.2 and P0.3 as SCL and SDA pins*/
	PINSEL0|=SCL_PIN|SDA_PIN;
	/*Set SCL low time*/
	I2SCLL=LOADVAL;
	/*Set SCL high time*/
	I2SCLH=LOADVAL;
	/*Enable I2C peripheral*/
	I2CONSET=1<<I2EN_BIT;
}
/*Generate START condition*/
void I2C_START()
{
	/*Generate START condition*/
	I2CONSET=1<<STA_BIT;
	/*Clear SI flag*/
	I2CONCLR=1<<SIC_BIT;
	/*Wait until START condition is transmitted*/
	while(((I2CONSET>>SI_BIT)&1)==0);
	/*Clear START bit*/
	I2CONCLR = 1<<STAC_BIT;
}
/* Generate STOP condition*/
void I2C_STOP()
{
	/*Generate STOP condition*/
	I2CONSET=1<<STO_BIT;
	//CLEAR SI flag*/
	I2CONCLR=1<<SIC_BIT;
}
/* Generate REPEATED START condition*/
void I2C_RESTART()
{
	/*Generate repeated START*/
	I2CONSET=1<<STA_BIT;
	/*Clear SI flag*/
	I2CONCLR=1<<SIC_BIT;
	/*Wait until START transmitted*/
	while(((I2CONSET>>SI_BIT)&1)==0);
	/*Clear START bit*/
	I2CONCLR = 1<<STAC_BIT;
}
/* Receive one byte and send NACK*/
char I2C_nack()
{
	/*Disable acknowledge*/
	I2CONCLR = 1<<AAC_BIT;
  /*Clear SI flag*/
  I2CONCLR=1<<SIC_BIT;
	/*Wait until byte received*/
  while(((I2CONSET>>SI_BIT)&1)==0);
	/*Return received byte*/
  return I2DAT;
}

char I2C_mack()
{
  /*Clear SI flag*/
  I2CONCLR=1<<SIC_BIT;
	/*Enable ACK*/
  I2CONSET = 1<<AA_BIT;
	/*Wait until data received*/
  while(((I2CONSET>>SI_BIT)&1)==0);
  /*Return received byte*/
  return I2DAT;
}
/*Send one byte over I2C*/
void I2C_write(char data1)
{
	/* Load data into transmit register*/
	I2DAT=data1;
	/* Clear SI flag to begin transmission*/
	I2CONCLR=1<<SIC_BIT;
  /*Wait until transmission completes*/
	while(((I2CONSET>>SI_BIT)&1)==0);
}
/* Write one byte to EEPROM*/
void i2c_byte_write(char slaveaddr,short int wbuffAddr,char dat)
{
	  /*Generate START condition*/
   	I2C_START();
	  /*Send slave address with write bit*/
	  I2C_write(slaveaddr<<1);
	  /*Send high byte of memory address*/
	  I2C_write(wbuffAddr>>8);
	  /*Send low byte of memory address*/
	  I2C_write(wbuffAddr&0XFF);
	  /*Send data byte*/
	  I2C_write(dat);
	  /*Generate STOP condition*/
	  I2C_STOP();
	  /*EEPROM write delay*/
	  delay_ms(10);
}
/*Read one byte from EEPROM*/
char i2c_random_read(char slaveaddr,short int RBuffAddr)
{ 
     char dat;
	   /*Generate START*/
     I2C_START();
	   /*Send slave address*/
	   I2C_write(slaveaddr<<1);
	   /*Send memory address high byte*/
	   I2C_write(RBuffAddr>>8);
	   /*Send memory address low byte*/
	   I2C_write(RBuffAddr&0XFF);
	   /*Generate repeated START*/
	   I2C_RESTART();
	   /*Send slave address*/
	   I2C_write((slaveaddr<<1)|1);
	   /* Read one byte with NACK*/
	   dat= I2C_nack();
	   /*Stop communication*/
	   I2C_STOP();
	   /*Return data*/
	   return dat;
}

void i2c_page_write(char slaveaddr,short int wBuffAddr,char *p,char nBytes)
{
     int j;
	 /*START condition*/
   I2C_START();
	 /*Slave address + write*/
	 I2C_write(slaveaddr<<1);
	 /*Memory address high byte*/
	 I2C_write(wBuffAddr>>8);
	 /*Memory address low byte*/
	 I2C_write(wBuffAddr&0XFF);
	 /*Write all bytes*/
	 for(j=0;j<nBytes;j++)
	 {
	   I2C_write(p[j]);
	 }
	 /*STOP condition*/
	 I2C_STOP();
	 /*EEPROM write delay*/
	 delay_ms(10);
}
/*Read multiple bytes from EEPROM*/
void i2c_seq_read(char slaveaddr,short int RBuffAddr,char *p,char nBytes)
{ 
     char j;
	 /*START condition*/
   I2C_START();
	 /*Send slave address*/
	 I2C_write(slaveaddr<<1);
	 /*Send memory address high byte*/
	 I2C_write(RBuffAddr>>8);
	 /*Send memory address low byte*/
	 I2C_write(RBuffAddr&0XFF);
	 /* Generate repeated START*/
	 I2C_RESTART();
	 /*Send slave address*/
	 I2C_write((slaveaddr<<1)|1);
	 /*Read all bytes except last with ACK*/
	 for(j=0;j<nBytes-1;j++)
	 {
	     p[j]=I2C_mack();
	 }
	 /*Read last byte with NACK*/
   p[j]=I2C_nack();
	 /*Generate STOP condition*/
	 I2C_STOP();
}					                                                                                                                      
 
