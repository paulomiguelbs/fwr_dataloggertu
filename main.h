#include <htc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h"
#include "protocolo.h"
#include "hardware.h"


/*******************************************************************************
 ************************** D E F I N I C O E S ********************************
 ******************************************************************************/
#define TRUE    1
#define FALSE   0
#define RUN     1
#define STOP    0

#define FW_VERSION_1 1 // "1"
#define FW_VERSION_2 0 // "0"
#define FW_VERSION_3 0 // "0"

/*******************************************************************************
 ***************************** F U N C O E S ***********************************
 ******************************************************************************/
bit checksumVerify (void);
unsigned char checksumCalc (unsigned char*, unsigned char);

/*******************************************************************************
 **************************** V A R I A V E I S ********************************
 ******************************************************************************/
//variavel de controle de tempo
unsigned long timeNow = 1641006000; //01/01/2022 00:00:00
unsigned long startTime = 1641006000;
unsigned int timer = 1; //10 segundos de fabrica
unsigned int newTimer = 1;
unsigned int contTimer;
unsigned int numOfLogs = 0;
bit logStatus = STOP;