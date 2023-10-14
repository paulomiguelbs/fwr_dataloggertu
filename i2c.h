#define ACK   0
#define NACK  1
#define SSP_MODULE

void i2cInitialize(const unsigned long feq_K); //Begin IIC as master
void i2cWait();
void i2cStart();
void i2cRestart();
void i2cStop();

#ifdef SSP_MODULE

#define DRIVE_SCL1_LOW		TRISB6=0		// PORTB 6 (SCL pin) mode = output, pulls line low
#define RELEASE_SCL1_HIGH	TRISB6=1		// PORT  6 (SCL pin) mode = input, line released high
#define DRIVE_SDA1_LOW   	TRISB4=0 		// PORTB 4 (SDA pin) mode = output, pulls line low
#define RELEASE_SDA1_HIGH	TRISB4=1		// PORTB 4 (SDA pin) mode = input, line released high
#define GET_SCL_PIN1		RB6				// PORTB 6 is used as SCL state read input
#define GET_SDA_PIN1		RB4				// PORTB 4 is used as SDA state read input


void scl_out (unsigned char value);
void sda_out (unsigned char value);
void halfclockdelay(void);
#endif

unsigned char i2cWrite(unsigned char);
unsigned char i2cRead(void);

void i2cAck(void);         //master receive only
void i2cNack(void);        //master receive only

void findDevice(void);