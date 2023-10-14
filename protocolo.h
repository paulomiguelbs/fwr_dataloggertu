/*******************************************************************************
 ************************** D E F I N I C O E S ********************************
 ******************************************************************************/
#define CMD_GET_CLOCK       0x31
#define CMD_SET_TIMER       0x32
#define CMD_GET_TIMER       0x33
#define CMD_START_LOG       0x34
#define CMD_STOP_LOG        0x35
#define CMD_READ_LOG_STATUS 0x36
#define CMD_READ_LOG_DATA   0x37
#define CMD_READ_LOG_DATA_X 0x38
#define CMD_READ_LOG_DATA_PAGE 0x39


#define START_TIME_DATA1_ADDR   0x0000
#define START_TIME_DATA2_ADDR   0x0001
#define START_TIME_DATA3_ADDR   0x0002
#define START_TIME_DATA4_ADDR   0x0003
#define TIMER_DATA1_ADDR        0x0004
#define TIMER_DATA2_ADDR        0x0005
#define NUM_OF_LOGS_DATA1_ADDR  0x0006
#define NUM_OF_LOGS_DATA2_ADDR  0x0007
#define LOG_STATUS_ADDR         0x0008

#define LOG_DATA_START_ADDR     0x0020


/*******************************************************************************
 ***************************** F U N C O E S ***********************************
 ******************************************************************************/
void getClockCmd (void);
void setTimerCmd (void);
void getTimerCmd (void);
void startLogCmd (void);
void stopLogCmd (void);
void readLogStatus (void);
void readLogData (void);
/*******************************************************************************
 **************************** V A R I A V E I S ********************************
 ******************************************************************************/
unsigned char bufferTx[40];
unsigned char bufferTxLen;