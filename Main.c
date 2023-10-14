/*
 * D:/ws-MPLAB/TU_DataLogger.X
 * TU_DataLogger
 * Project Type: Application - Configuration: default
 * Device
 * PIC16F690
 * Checksum: 0x909A
 * CRC32: 0x8F3BD5C5
 * Packs
 * PIC16Fxxx_DFP (1.3.42)
 * hi-tech-picc (v9.83) [C:\Program Files (x86)\HI-TECH Software\PICC\9.83\bin] 
 */


#include "main.h"
#include "i2c.h"
#include "eepromI2c.h"
#include "aht10.h"
__CONFIG(FOSC_INTRCIO & WDTE_OFF & PWRTE_OFF & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_ON & IESO_ON & FCMEN_ON);

/*******************************************************************************
 ***************************** F U N Ç Õ E S ***********************************
 ******************************************************************************/
void interrupt isr();
void initGPIO(void);
void initInterrupt(void);
void initVars(void);
void jdyConfig (void);

void comandoRx (void);
void logSave(void);
void resposta(void);

void testeMemoria(void);


/*******************************************************************************
 **************************** V A R I Á V E I S ********************************
 ******************************************************************************/

unsigned char readUmidity;
unsigned char readTemperature;
unsigned char readTemperatureDec;

unsigned char timeToConnect = 0;
unsigned char timeBlinkLed = 0;

unsigned char readDataBuffer[30];

bit flagConection = FALSE;
bit flagRespostaBLE = FALSE;
bit flagTryConnect = FALSE;
/*******************************************************************************
 ********************* F U N Ç Ã O   P R I N C I P A L *************************
 ******************************************************************************/
int main(int argc, char** argv) {
    initGPIO();
    initUART();
    i2cInitialize(1000);
    initInterrupt();
    PWRC_PIN = 1; //1 habilita o modulo BLE para transmissao serial
    __delay_ms(100);
    initVars();
    initAht10();
    __delay_ms(5);
    jdyConfig();    
    __delay_ms(250);
    memset(rxBuffer,0,RX_BUFFER_SIZE);
    //testeMemoria();
    while(1){        
        if(logStatus == RUN && contTimer == 0){
            contTimer = timer;
            //grava um log   
            logSave();           
        }
        
        if (flagRxBufferOk){
            flagRxBufferOk = 0;
            if (rxBufferLength >= 3){
                if (rxBuffer[2]+5 == rxBufferLength){
                    if (rxBuffer[0] == 0x55 && rxBuffer[rxBufferLength-1] == 0x0A){                
                       comandoRx();
                       rxBufferPos = 0;
                       memset(rxBuffer,0,rxBufferLength);
                    }
                }
            }
        }
        
        if(STAT_PIN == 1){
            flagConection = TRUE;
            timeToConnect = 0;
            PWRC_PIN = 1;
            LED_BLUE = LED_ON;
        }
        if (STAT_PIN == 0 && timeToConnect > 0){
            if (timeBlinkLed == 0){
                timeBlinkLed = TIME_BLINK_LED;
                LED_BLUE = !LED_BLUE;
                
            }
        }
        
        
        if(STAT_PIN == 0 && timeToConnect == 0){
            LED_BLUE = LED_OFF;
            if (flagTryConnect == TRUE || flagConection == TRUE){
                flagConection = FALSE;
                flagTryConnect = FALSE;
                PWRC_PIN = 0; // habilita o modulo BLE para comando AT
                __delay_ms(100);
                UART_send_String("AT+SLEEP2\r\n",11); //sleep mode with no broadcast
                __delay_ms(350);
                PWRC_PIN = 1;
            }
            asm("SLEEP");
        }
    }
    return (EXIT_SUCCESS);
}


void testeMemoria(void){
    unsigned int addr=0x0020;
    unsigned char data = 0x20;
    unsigned int i;
    unsigned int j=0;
    for (i=0 ; i< 32736; i++){
        eepromWriteByte(addr+i,data+j);
        j++;
        if (j == 10){
            j = 0;
            data = 0x20;
        }
        __delay_ms(5);  
    }
    
}
void jdyConfig (void){
    PWRC_PIN = 0; // habilita o modulo BLE para comando AT
    __delay_ms(100);
    UART_send_String("AT+ADVIN1\r\n",11); //200ms broadcast interval
    __delay_ms(350);
    UART_send_String("AT+STARTEN1\r\n",13);
    __delay_ms(350);    
    UART_send_String("AT+RST\r\n",8);
    __delay_ms(350); 
    flagTryConnect = TRUE;
    PWRC_PIN = 1; // coloco a serial do modulo para transmissao serial.
}
/*******************************************************************************
 * SALVA UM LOG NA MEMORIA *****************************************************
 ******************************************************************************/
void logSave(void){
     if (numOfLogs < MAX_NUM_OF_LOG_EVENT){
        unsigned int auxAddr;
        
        auxAddr = LOG_DATA_START_ADDR+(numOfLogs*3);
        
        sendTriggerMeasurementCmd();
        __delay_ms(100);
        readTemp();
        
        //grava a temperatura e umidade na memoria
        eepromWriteByte(auxAddr,readTemperature);
        auxAddr++;
        __delay_ms(5);            
        eepromWriteByte(auxAddr,readTemperatureDec);
        auxAddr++;
        __delay_ms(5);
        eepromWriteByte(auxAddr,readUmidity);       
        __delay_ms(5);
        
        //grava o novo numero de logs na memoria
        numOfLogs++;
        eepromWriteByte(NUM_OF_LOGS_DATA1_ADDR,numOfLogs>>8);
        __delay_ms(5); 
        eepromWriteByte(NUM_OF_LOGS_DATA2_ADDR,numOfLogs);
        __delay_ms(5);
     }    
}


/*******************************************************************************
 ********************* TRATAMENTO DO COMANDO SERIAL ****************************
 ******************************************************************************/
void comandoRx(void) {
    unsigned char chk = 0;
    unsigned long numOfBytes = 0;
    unsigned int logsToSend = 0;
    if (checksumVerify()){
        memcpy(cmd, rxBuffer, rxBufferLength);
            switch (cmd[1]){
                case CMD_GET_CLOCK:
                    getClockCmd();
                    flagRespostaBLE = TRUE;
                    break;

                case CMD_SET_TIMER:             
                    setTimerCmd();
                    flagRespostaBLE = TRUE;
                    break;                

                case CMD_GET_TIMER:
                    getTimerCmd();
                    flagRespostaBLE = TRUE;
                    break;

                case CMD_START_LOG:
                    startLogCmd();
                    flagRespostaBLE = TRUE;
                    break;

                case CMD_STOP_LOG:
                    stopLogCmd();
                    flagRespostaBLE = TRUE;
                    break;

                case CMD_READ_LOG_STATUS:
                    readLogStatus();
                    flagRespostaBLE = TRUE;
                    break;                   
                    
                case CMD_READ_LOG_DATA:
                    chk = 0;
                    bufferTxLen = 14;
                    bufferTx[0] = 0x55;
                    bufferTx[1] = CMD_READ_LOG_DATA;
                    chk ^= bufferTx[1];
                    logsToSend = numOfLogs;
                    numOfBytes = logsToSend*3;
                    numOfBytes+= 9;
                    bufferTx[2] = (numOfBytes>>16)&0xFF; //numero de bytes que vao ser enviados
                    chk ^= bufferTx[2];
                    bufferTx[3] = (numOfBytes>>8)&0xFF;
                    chk ^= bufferTx[3];
                    bufferTx[4] = (numOfBytes&0xFF);
                    chk ^= bufferTx[4];
                    
                    bufferTx[5] = startTime>>24;
                    chk ^= bufferTx[5];
                    bufferTx[6] = startTime>>16;
                    chk ^= bufferTx[6];
                    bufferTx[7] = startTime>>8;
                    chk ^= bufferTx[7];
                    bufferTx[8] = startTime;
                    chk ^= bufferTx[8];
                    bufferTx[9] = timer>>8;
                    chk ^= bufferTx[9];
                    bufferTx[10] = timer;
                    chk ^= bufferTx[10];
                    bufferTx[11] = logsToSend>>8;
                    chk ^= bufferTx[11];
                    bufferTx[12] = logsToSend;
                    chk ^= bufferTx[12];
                    bufferTx[13] = logStatus;    
                    chk ^= bufferTx[13];
                    resposta();
                    
                    for (unsigned int i=0; i<logsToSend; i++){
                        unsigned char readData[3];                        
                        bufferTxLen = 3;                        
                        eepromReadLog(LOG_DATA_START_ADDR+(3*i),readData);
                        bufferTx[0] = readData[0]; //temp
                        bufferTx[1] = readData[1]; //temp decimal
                        bufferTx[2] = readData[2]; //umidade                       
                        chk ^= bufferTx[0];
                        chk ^= bufferTx[1];
                        chk ^= bufferTx[2];
                        resposta();
                    }
                    bufferTx[0] = chk;
                    bufferTx[1] = 0x0A;
                    bufferTxLen = 2;
                    resposta();
                    break;
                    
                case CMD_READ_LOG_DATA_X:
                    logsToSend = cmd[3];                    
                    logsToSend <<=8;
                    logsToSend += cmd[4];
                    unsigned char readData[3];
                    bufferTxLen = 8;
                    eepromReadLog(LOG_DATA_START_ADDR+(logsToSend*3),readData);
                    
                    bufferTx[0] = 0x55;
                    bufferTx[1] = CMD_READ_LOG_DATA_X; 
                    bufferTx[2] = 0x03;
                    bufferTx[3] = readData[0];
                    bufferTx[4] = readData[1];
                    bufferTx[5] = readData[2];
                    bufferTx[6] = checksumCalc(bufferTx,8);                    
                    bufferTx[7] = 0x0A;
                    resposta();
                    break;
                    
                case CMD_READ_LOG_DATA_PAGE:
                    logsToSend = cmd[3];                    
                    logsToSend <<=8;
                    logsToSend += cmd[4];

                    bufferTxLen = 37;
                    eepromReadLogPage(LOG_DATA_START_ADDR+(30*logsToSend),readDataBuffer);
                    
                    bufferTx[0] = 0x55;
                    bufferTx[1] = CMD_READ_LOG_DATA_PAGE; 
                    bufferTx[2] = 0x20;
                    bufferTx[3] = cmd[3];
                    bufferTx[4] = cmd[4];
                    
                    for(int i=0;i<30;i++){
                        bufferTx[i+5] = readDataBuffer[i];
                    }
                    bufferTx[35] = checksumCalc(bufferTx,37);                    
                    bufferTx[36] = 0x0A;
                    resposta();
                    break;
                default:
                    break;
            }
            if (flagRespostaBLE){
                flagRespostaBLE = FALSE;
                resposta();
            }           
        }
        else{
            UART_send_char(0x55);
            UART_send_char(0x40);
            UART_send_char(0x00);
            UART_send_char(0x40);
            UART_send_char(0x0A);
        }
 
}
void resposta (void){
    for (char i=0 ; i<bufferTxLen ; i++){
        UART_send_char(bufferTx[i]);
    }  
}
/*******************************************************************************
 ************************** INICIA AS VARIAVEIS ********************************
 ******************************************************************************/
void initVars(void){
    //Cerragando o timer
    timer = eepromReadByte(TIMER_DATA1_ADDR);
    timer<<=8;
    __delay_ms(5);
    timer += eepromReadByte(TIMER_DATA2_ADDR);
    __delay_ms(5);
    
    contTimer = timer;
    newTimer = timer;
    
    //Carregando o horario incicial
    startTime = eepromReadByte(START_TIME_DATA1_ADDR);
    startTime<<=8;
    __delay_ms(5);
    startTime += eepromReadByte(START_TIME_DATA2_ADDR);
    startTime<<=8;
    __delay_ms(5);
    startTime += eepromReadByte(START_TIME_DATA3_ADDR);
    startTime<<=8;
    __delay_ms(5);
    startTime += eepromReadByte(START_TIME_DATA4_ADDR);
    __delay_ms(5);
    
    //Carregando o numero de logs gravados
    numOfLogs = eepromReadByte(NUM_OF_LOGS_DATA1_ADDR);
    __delay_ms(5);
    numOfLogs <<= 8;
    numOfLogs += eepromReadByte(NUM_OF_LOGS_DATA2_ADDR);
    __delay_ms(5);
}

/*******************************************************************************
 ************************** VALIDAÇÃO DO CHECKSUM ******************************
 ******************************************************************************/
bit checksumVerify(void){
    unsigned char len = rxBufferLength;
    unsigned char checksum = 0;

    for (unsigned char i=1 ;i<len-2;i++){
        checksum ^= rxBuffer[i];        
    }
    if(rxBuffer[len-2] == checksum){
        
        return TRUE;
    }
    else
        return FALSE;
}

/*******************************************************************************
 ************************** VALIDAÇÃO DO CJECKSUM ******************************
 ******************************************************************************/
unsigned char checksumCalc(unsigned char* buffer, unsigned char len){
    unsigned char chk = 0;
    for (char i = 1; i< len-2; i++){
        chk ^= buffer[i]; 
    }    
    return chk;
}

/*******************************************************************************
 ************************** I N T E R R U P Ç Ã O ******************************
 ******************************************************************************/
void interrupt isr(){
    if(RCIE == 1 && RCIF == 1){
        dataRX = UART_get_char();
        bufferCreator2(dataRX);
    }
    if (INTE == 1 && INTF == 1){ //interrupção externa para conectar BLE
        INTF = 0;
        //flagWakeUp = TRUE;
        flagTryConnect = TRUE;
        timeToConnect = TIME_TO_CONNECT;
        timeBlinkLed = TIME_BLINK_LED;
        PWRC_PIN = 0; // acorda o o modulo ble
    }
    if (T0IE == 1 && T0IF == 1){
        T0IE = 1;
        T0IF = 0;
        TMR0 = 60; 
        if (serialTimeOut > 0){serialTimeOut--;}
        if (serialTimeOut == 0){
            serialTimeOut = SERIAL_TIMEOUT;
            rxBufferPos = 0;
            rxBufferLength = 0;
            memset(rxBuffer,0,rxBufferLength);
        }
        if (timeToConnect>0){
            timeToConnect--;
        }
        
        if (timeBlinkLed>0){
            timeBlinkLed--;
        }
    }
    if (TMR1IE == 1 && TMR1IF == 1){ // timer 10segundos oscilador externo
        TMR1IF = 0;
        TMR1ON = 0;
        TMR1L = 0x00;    //valores inciais
        TMR1H = 0x60;    //valores inciais      
        TMR1ON = 1;              
        timeNow += 10;
        if (contTimer > 0){
            contTimer --;            
        }
    }
}

/*******************************************************************************
 *********************** I N I C I A L I Z A   G P I O S ***********************
 ******************************************************************************/
void initGPIO(void){
    ANSEL = 0x00;
    ANSELH = 0x00;
    PORTA = 0;
    TRISA = 0b00000100;
    PORTA = 0;   
    
    PORTB = 0;
    TRISB = 0b00000000;
    PORTB = 0;
    
    PORTC = 0;
    TRISC = 0b00000001;
    PORTC = 0;
}

/*******************************************************************************
 **************** I N I C I A L I Z A  I N T E R R U P Ç Õ E S *****************
 ******************************************************************************/
void initInterrupt(void){
    GIE = 1;
    PEIE = 1;
    
    //interrupção externa
    INTE = 1;
    INTF = 0;
    INTEDG = 1; //rising edge
    
    
    //interrupção timer 0
    T0CS = 0; //clock interno/4
    PSA = 0;  //prescaler para o timer 0
    PS0 = 1;
    PS1 = 1;
    PS2 = 1;  //    1/256
    TMR0 = 12;
    T0IF = 0;
    T0IE = 1;
    
    //tempo = 1e-6 * 256* 244 = 62,464ms -> 16 ciclos para 1s
    
    //interrupção timer1
    TMR1L = 0x00;    //valores inciais
    TMR1H = 0x60;   //valores inciais
    T1CKPS0 = 1;
    T1CKPS1 = 1;
    TMR1CS = 1;      //fonte esterna para o clock
    nT1SYNC = 1;     //contador cloc externo assincrono
    T1OSCEN = 1;     //LP oscilator habilitado para Timer1
    TMR1IF = 0;
    TMR1IE = 1;
    TMR1ON = 1;
}