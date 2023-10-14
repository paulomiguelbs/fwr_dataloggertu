#include "main.h"
/*******************************************************************************
 *INICIALIZA COMUNICAÇÃO ASSINCRONA ********************************************
 *FOSC: 4MHZ *******************************************************************
 *BAUD RATE: 9600 **************************************************************
 ******************************************************************************/
void initUART (void){
    // IOs settings
    TRISB5 = 1; // RX = INPUT
    TRISB7 = 0; // TX = OUTPUT
    
    //Baud Rate Generator settings
    BRG16 = 0;  // 8bits baud rate generator 
    BRGH = 1;   // High Baud Rate Select bit High Speed    
    SPBRGH = 0;
    SPBRG = 25;
    
    // assyncronous mode
    SYNC = 0; //assyncronous mode
    SPEN = 1; //enable serial port
    
    // prepared to TX and RX
    RCIE = 1;
    TXEN = 1;
    CREN = 1;
    
    //8 bit mode
    TX9 = 0;
    RX9 = 0;
}

/*******************************************************************************
 *Envia um character pela serial ***********************************************
 * ch = caracter a ser enviado. ************************************************
 ******************************************************************************/
void UART_send_char(char ch)  
{
    while(!TXIF);  // hold the program till TX buffer is free
    asm("NOP");
    asm("NOP");
    
    TXREG = ch; //Load the transmitter buffer with the received value
}

/*******************************************************************************
 *Envia uma string pela serial *************************************************
 * ch = primeiro endereço da string. *******************************************
 ******************************************************************************/
void UART_send_String(char* ch, unsigned char lenth){
    /*
    while(*ch){ //if there is a char
        UART_send_char(*ch++); //process it as a byte data
    }
    */
    
    for(char i=0; i<lenth ; i++){
        UART_send_char(*ch++);
    }
}

/*******************************************************************************
 *Retorna a leitura de um byte serial ***********************************************
 *************************************************
 ******************************************************************************/
char UART_get_char() 
{    
    if(OERR){ // check for Error         
        CREN = 0; //If error -> Reset 
        CREN = 1; //If error -> Reset 
    }       
    return RCREG; //receive the value and send it to main function
    
    //INSERIR A SEGUINTE INTERRUPÇÃO 
    /*if(RCIE == 1 && RCIF == 1){
        char dataRX = UART_get_char();        
    }*/
}

/*******************************************************************************
 *Monta o rxBuffer *************************************************************
 *******************************************************************************
 ******************************************************************************/
void bufferCreator (char ch){
    if (rxBufferPos == 0){
        memset(rxBuffer,0,sizeof(rxBuffer));
    }
    if (rxBufferPos < RX_BUFFER_SIZE-1){
        if (ch != '\n'){
            rxBuffer[rxBufferPos] = ch;            
            rxBufferPos++;
        }
        else{
            flagRxBufferOk = 1;            
            //rxBuffer[rxBufferPos] = ch;
            rxBufferLength = rxBufferPos;
            rxBufferPos = 0;
        }
       
    }
    else{
        rxBufferPos = 0;
        memset(rxBuffer,0,sizeof(rxBuffer));

    }
}

void bufferCreator2 (char ch){
    /*
     
    
    if (rxBufferPos == 0){
        memset(rxBuffer,0,sizeof(rxBuffer));
    }
     
    */
    
    if (rxBufferPos < RX_BUFFER_SIZE-1){
        //UART_send_char(ch);
        rxBuffer[rxBufferPos] = ch;            
        rxBufferPos++;
        rxBufferLength = rxBufferPos;
        flagRxBufferOk = 1;
        serialTimeOut = SERIAL_TIMEOUT;        
    }
    else{
        rxBufferPos = 0;
        memset(rxBuffer,0,rxBufferLength);
    }
}