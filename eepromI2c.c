#include <htc.h>
#include "i2c.h"
#include "eepromI2c.h"
#include "uart.h"
#include "hardware.h"

unsigned char eepromReadByte(unsigned int addr){
    unsigned char buff;
    i2cStart();
    i2cWrite(I2C_EEPROM_ADDR_W);
    i2cWrite(addr>>8);
    i2cWrite((unsigned char)addr);
    i2cRestart();
    i2cWrite(I2C_EEPROM_ADDR_R);
    buff = i2cRead();
    i2cNack();
    i2cStop();
    
    return buff;
}

void eepromWriteByte(unsigned int addr, unsigned char data){
    i2cStart();
    i2cWrite(I2C_EEPROM_ADDR_W);
    i2cWrite(addr>>8);
    i2cWrite((unsigned char)addr);
    i2cWrite(data);
    i2cStop();
}

void eepromReadLog(unsigned int addr, unsigned char* buff){
    i2cStart();
    i2cWrite(I2C_EEPROM_ADDR_W);
    i2cWrite(addr>>8);
    i2cWrite((unsigned char)addr);
    i2cRestart();
    i2cWrite(I2C_EEPROM_ADDR_R);
    buff[0] = i2cRead();
    i2cAck();
    buff[1] = i2cRead();
    i2cAck();
    buff[2] = i2cRead();
    i2cNack();
    i2cStop();
}

void eepromReadLogPage(unsigned int addr, unsigned char* buff){
    i2cStart();
    i2cWrite(I2C_EEPROM_ADDR_W);
    i2cWrite(addr>>8);
    i2cWrite((unsigned char)addr);
    i2cRestart();
    i2cWrite(I2C_EEPROM_ADDR_R);    
    for (int i = 0; i<30 ; i++){
        buff[i] = i2cRead();
        if (i<29){
            i2cAck();
        }        
    }
    i2cNack();
    i2cStop();
}
