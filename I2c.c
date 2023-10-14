#include "main.h"
//#include <htc.h>
#include "i2c.h"
#include "eepromI2c.h"
//#include "uart.h"
//#include "hardware.h"
#define _XTAL_FREQ 4000000



#ifdef MSSP_MODULE
void i2cInitialize(const unsigned long feq_K) //Begin IIC as master
{
  TRISC3 = 1;  TRISC4 = 1;  //Set SDA and SCL pins as input pins
  SSPCON  = 0b00101000;    //pg84/234
  SSPCON2 = 0b00000000;    //pg85/234

  SSPADD = (_XTAL_FREQ/(4*feq_K*100))-1; //Setting Clock Speed pg99/234
  SSPSTAT = 0b00000000;    //pg83/234
}

void i2cWait()
{
    while (   (SSPCON2 & 0b00011111)    ||    (SSPSTAT & 0b00000100)   ) ; //check the this on registers to make sure the IIC is not in progress
}

void i2cRestart()
{
  i2cWait();
  RSEN = 1;
}

void i2cStart()
{
  i2cWait();  //Hold the program is I2C is busy 
  SEN = 1;     //Begin IIC pg85/234
}

void i2cStop()
{
  i2cWait(); //Hold the program is I2C is busy 
  PEN = 1;    //End IIC pg85/234
}

unsigned char i2cWrite(unsigned char data)
{
  i2cWait(); //Hold the program is I2C is busy
  SSPBUF = data;         //pg82/234
  i2cWait();
  return ACKSTAT;
}
unsigned char i2cRead (void)
{
  unsigned char incoming;
  i2cWait();
  RCEN = 1;

  i2cWait();
  incoming = SSPBUF;      //get the data saved in SSPBUF

  i2cWait();     //pg 85/234

  return incoming;
}
void i2cAck(void)
{
  //---[ Send ACK - For Master Receiver Mode ]---
  i2cWait();
  ACKDT = 0; // 0 -> ACK, 1 -> NACK
  ACKEN = 1; // Send ACK Signal!
}

//Send NACK Signal (For Master Receiver Mode Only)
void i2cNack(void)
{
  //---[ Send NACK - For Master Receiver Mode ]---
  i2cWait();
  ACKDT = 1; // 1 -> NACK, 0 -> ACK
  ACKEN = 1; // Send NACK Signal!
}

#endif

#ifdef SSP_MODULE
void i2cInitialize(const unsigned long feq_K) //Begin IIC as master
{
  TRISB4 = 1;  TRISB6 = 1;  //Set SDA and SCL pins as input pins
  //SSPCON  = 0b00101000;    //pg84/234
  //SSPCON2 = 0b00000000;    //pg85/234

  //SSPADD = (_XTAL_FREQ/(4*feq_K*100))-1; //Setting Clock Speed pg99/234
  //SSPSTAT = 0b00000000;    //pg83/234
}

void i2cWait()
{
    //while ((SSPSTAT & 0b00000100)   ) ; //check the this on registers to make sure the IIC is not in progress
}

void halfclockdelay (void)
{
    
	unsigned char delayvalue = 0x10;	// A completely arbitrary value.
	unsigned char downcount;
	for (downcount = delayvalue; downcount >0; downcount--);
     
   // asm("NOP");
}

void i2cRestart()
{
    i2cStart();
}

void i2cStart()
{
   scl_out(1);  // SCL high 
   sda_out(1);  // SDA high   
   sda_out(0);  // SDA low, indicates START  
   scl_out(0);  // SCL low                   
}

void i2cStop()
{
   sda_out(0);  // SDA low                   
   scl_out(1);  // SCL high                  
   sda_out(1);  // SDA high, indicates STOP  
}

unsigned char i2cWrite(unsigned char data)
{
    unsigned char x,ack;
    for (x=0; x<=7; x++){ // loop 8 times      
     sda_out((data & 0x80) != 0); 	    // msb first  
     scl_out(1);            			// assert clock        
     scl_out(0);            			// de-assert clock     
     data <<= 1;                		// shift left one      
    }
    sda_out(1);             // release SDA for ack           
    scl_out(1);             // assert clock 
    ack = GET_SDA_PIN1;     // get ack status
    scl_out(0);      // de-assert clock             
    return(ack);  
}

unsigned char i2cRead (void)
{
    unsigned char x,read_data;
    read_data = 0;
    for (x=0; x<=7; x++){
        read_data <<= 1;            // shift left one      
        scl_out(1);                 // SCL high            
        read_data |= GET_SDA_PIN1;  // read data bit
        scl_out(0); 
    }
                        // SCL low             
    return(read_data);
}

void i2cAck(void)
{
  sda_out(0);               // ACK    
  scl_out(1);               // SCL high            
  scl_out(0);               // SCL low             
  sda_out(1);               // ACK de-asserted 
}

void i2cNack(void)
{
    sda_out(1);               // ACK 
    scl_out(1);               // SCL high      
    scl_out(0);               // SCL low
    sda_out(1);               // ACK de-asserted 
}

void scl_out(unsigned char value)
{
    if(value==1){   // release clock high
        RELEASE_SCL1_HIGH; 
            // wait for clock line high for clock stretching
        while(GET_SCL_PIN1 == 0);
    }
    else{   // drive clock low
        DRIVE_SCL1_LOW; 
    }
    //asm("NOP");
    //asm("NOP");
    halfclockdelay();
}  

void sda_out(unsigned char value)
{
    if(value==1){
        RELEASE_SDA1_HIGH;
    }
    else{
        DRIVE_SDA1_LOW;
    }
    //asm("NOP");
    //asm("NOP");
    halfclockdelay();
} 

#endif












void findDevice(void){
    for(char i=1; i<128; i++){
        i2cStart();
        char dev = i2cWrite(i<<1);
        i2cStop();
        if (dev == ACK){
            UART_send_char(i<<1);
            UART_send_char('\n');
            __delay_ms(30);
        }
    }
}