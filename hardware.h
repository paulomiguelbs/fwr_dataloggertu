/*******************************************************************************
 ************************** D E F I N I Ç Õ E S ********************************
 ******************************************************************************/

#define _XTAL_FREQ 4000000

//GPIOS
#define STAT_PIN    RC0     //INPUT
#define PWRC_PIN    RC4     //OUTPUT

#define LED_BLUE    RC1     //OUTPUT
#define BT1         RA2     //INPUT

#define LED_ON  0
#define LED_OFF 1

#define TIME_TO_CONNECT 200 //10s base 50ms
#define TIME_BLINK_LED  5 //250ms base 50ms