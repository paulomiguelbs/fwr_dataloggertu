#include "aht10.h"
#include "i2c.h"
#include "uart.h"



extern unsigned char readUmidity;
extern unsigned char readTemperature;
extern unsigned char readTemperatureDec;


void initAht10 (void){
    i2cStart();
    i2cWrite(0x70);
    i2cWrite(0xE1);
    i2cWrite(0x08);
    i2cWrite(0x00);
    i2cStop();
}

unsigned char readStatusBitAht10 (void){
    unsigned char buff;
    i2cStart();
    i2cWrite(0x71);
    buff = i2cRead();
    i2cNack();
    i2cStop();
    return buff;
}



void sendTriggerMeasurementCmd (void){
    i2cStart();
    i2cWrite(0x70);
    i2cWrite(0xAC);
    i2cWrite(0x33);
    i2cWrite(0x00);
    i2cStop();    
}

char readTemp (void){
    
    unsigned long aux;
    
    unsigned char buff[6];
    i2cStart();
    i2cWrite(0x71);
    buff[0] = i2cRead();    
     // sensor is busy or not calibrated
    if( ((buff[0]&0x80)>>7 == 1) || ((buff[0]&0x08)>>3 == 0) ){
       i2cNack();
       i2cStop();
       return 0;
    }
    else{
        i2cAck();
    }    
    
    buff[1] = i2cRead();
    i2cAck();

    buff[2] = i2cRead();
    i2cAck();

    buff[3] = i2cRead();
    i2cAck();

    buff[4] = i2cRead();
    i2cAck();

    buff[5] = i2cRead();
    i2cNack();
    i2cStop();    
    
    aux = 0x00;    
    aux = buff[1];
    aux <<= 8;
    aux += buff[2];
    aux <<= 8;
    aux += buff[3];
    aux >>= 4;    
        
    aux *= 100;
    aux >>=20;  
        
    readUmidity = aux/10;
    readUmidity <<= 4;
    aux %= 10;
    readUmidity+=aux;

    aux = 0;
    aux = (buff[3] & 0x0F);
    aux <<= 8;
    aux += buff[4];
    aux <<= 8;
    aux += buff[5];
    
    aux *= 1000;
    aux >>= 10;
    aux *= 200;
    aux >>= 10;
    aux -= 50000;
    
    unsigned char aux2;
    
    aux2 = aux/10000;
    aux2 <<= 4;    
    readTemperature = aux2;    
    aux2 = (aux/1000)%10;    
    readTemperature += aux2;
    
    readTemperatureDec = (aux/100)%10;
    readTemperatureDec <<= 4;
    readTemperatureDec += (aux/10)%10;
    return 1;

}
