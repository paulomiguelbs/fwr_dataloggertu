#include "main.h"
#include "eepromI2c.h"

#define TRUE    1
#define FALSE   0
#define RUN     1
#define STOP    0


/*******************************************************************************
 ***************************** F U N Ç Õ E S ***********************************
 ******************************************************************************/
void getClockCmd (void){    
    //monta o buffer de resposta
    bufferTx[0] = 0x55;
    bufferTx[1] = CMD_GET_CLOCK;
    bufferTx[2] = 0x04;
    bufferTx[3] = timeNow>>24;
    bufferTx[4] = timeNow>>16;
    bufferTx[5] = timeNow>>8;
    bufferTx[6] = timeNow + (( TMR1H-0x61) >> 4);
    bufferTx[7] = checksumCalc(bufferTx,9);
    bufferTx[8] = 0x0A;
    bufferTxLen = 9;
}

/*******************************************************************************
 **SET TIMER COMMAND **********************************************************/
void setTimerCmd (void){
    //atualiza novos valores
    newTimer = cmd[3];
    newTimer<<=8;
    newTimer += cmd[4];      
    
    //responde no BLE
    bufferTxLen = 5;
    bufferTx[0] = 0x55;
    bufferTx[1] = CMD_SET_TIMER;
    bufferTx[2] = 0x00;
    bufferTx[3] = checksumCalc(bufferTx,5);
    bufferTx[4] = 0x0A;
}

/*******************************************************************************
 **GET TIMER COMMAND **********************************************************/
void getTimerCmd (void){
    bufferTxLen = 7;
    bufferTx[0] = 0x55;
    bufferTx[1] = CMD_GET_TIMER;
    bufferTx[2] = 0x02;
    bufferTx[3] = timer>>8;
    bufferTx[4] = timer;
    bufferTx[5] = checksumCalc(bufferTx,7);
    bufferTx[6] = 0x0A;
}

/*******************************************************************************
 **START LOG COMMAND **********************************************************/
void startLogCmd (void){ 
    
    //reset do relogio interno
    TMR1ON = 0;
    TMR1L = 0x00;    //valores inciais
    TMR1H = 0x60;    //valores inciais 
    TMR1IF = 0;
    TMR1ON = 1;   
    
    //atualiza novos valores
    startTime = cmd[3];
    startTime <<= 8;
    startTime +=cmd[4];
    startTime <<= 8;
    startTime +=cmd[5];
    startTime <<= 8;
    startTime +=cmd[6];
    timeNow = startTime;
    timer = newTimer;
    contTimer = timer;
    logStatus = RUN;
    numOfLogs = 0;
    
    //apaga os logs
    eepromWriteByte(NUM_OF_LOGS_DATA1_ADDR,0x00);
    __delay_ms(5); 
    eepromWriteByte(NUM_OF_LOGS_DATA2_ADDR,0x00);
    __delay_ms(5); 
     
    //grava o valor do new timer
    eepromWriteByte(TIMER_DATA1_ADDR,timer>>8);
    __delay_ms(5);
    eepromWriteByte(TIMER_DATA2_ADDR,timer);
    __delay_ms(5);
    
    //grava o horario inicial
    eepromWriteByte(START_TIME_DATA1_ADDR,cmd[3]);
    __delay_ms(5);  
    eepromWriteByte(START_TIME_DATA2_ADDR,cmd[4]);
    __delay_ms(5);  
    eepromWriteByte(START_TIME_DATA3_ADDR,cmd[5]);
    __delay_ms(5);  
    eepromWriteByte(START_TIME_DATA4_ADDR,cmd[6]);
    __delay_ms(5);
    
    //responde no BLE
    bufferTxLen = 5;
    bufferTx[0] = 0x55;
    bufferTx[1] = CMD_START_LOG;
    bufferTx[2] = 0x00;
    bufferTx[3] = checksumCalc(bufferTx,5);
    bufferTx[4] = 0x0A;
}

/*******************************************************************************
 **START LOG COMMAND **********************************************************/
void stopLogCmd (void){    
    //atualiza novos valores
    logStatus = STOP;
    
    //responde no BLE
    bufferTxLen = 5; 
    bufferTx[0] = 0x55;
    bufferTx[1] = CMD_STOP_LOG;
    bufferTx[2] = 0x00;
    bufferTx[3] = checksumCalc(bufferTx,5);
    bufferTx[4] = 0x0A;
}

/*******************************************************************************
 * STOP LOG COMMAND ***********************************************************/
void readLogStatus (void){    
    //responde no BLE
    bufferTxLen = 14;
    bufferTx[0] = 0x55;
    bufferTx[1] = CMD_READ_LOG_STATUS;
    bufferTx[2] = 0x09;
    bufferTx[3] = startTime>>24;
    bufferTx[4] = startTime>>16;
    bufferTx[5] = startTime>>8;
    bufferTx[6] = startTime;
    bufferTx[7] = timer>>8;
    bufferTx[8] = timer;
    bufferTx[9] = numOfLogs>>8;
    bufferTx[10] = numOfLogs;
    bufferTx[11] = logStatus;    
    bufferTx[12] = checksumCalc(bufferTx,14);
    bufferTx[13] = 0x0A;
}

/*******************************************************************************
 **SET TIMER COMMAND **********************************************************/
void readLogData (void){
    unsigned char readData[3];
    for (unsigned int i=0; i<numOfLogs; i++){  
        eepromReadLog(LOG_DATA_START_ADDR+(3*i),readData);        
        //responde no BLE
        bufferTxLen = 9;
        bufferTx[0] = 0x55;
        bufferTx[1] = CMD_READ_LOG_DATA;
        bufferTx[2] = i>>8; //numero do log        
        bufferTx[3] = i; // numero do log
        bufferTx[4] = readData[0];
        bufferTx[5] = readData[1]; //temp decimal
        bufferTx[6] = readData[2]; //umidade
        bufferTx[7] = checksumCalc(bufferTx,9);
        bufferTx[8] = 0x0A;    
    }    
}
