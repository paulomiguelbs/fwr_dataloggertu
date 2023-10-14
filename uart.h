/*******************************************************************************
 ************************** D E F I N I � � E S ********************************
 ******************************************************************************/
#define RX_BUFFER_SIZE 30
#define LF 0
#define CHARATCER 1

#define SERIAL_TIMEOUT 10
/*******************************************************************************
 ***************************** F U N � � E S ***********************************
 ******************************************************************************/
void initUART(void);
void UART_send_char(char);
void UART_send_String(char*, unsigned char);
char UART_get_char(void);
void bufferCreator(char);
void bufferCreator2(char);

/*******************************************************************************
 **************************** V A R I � V E I S ********************************
 ******************************************************************************/
char rxBuffer[RX_BUFFER_SIZE];
char cmd[RX_BUFFER_SIZE];
char rxBufferLength;
char dataRX;
char rxBufferPos = 0;
char serialTimeOut = 0;
bit flagRxBufferOk;
