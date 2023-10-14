#define I2C_AHT10_ADDR_W 0x70
#define I2C_AHT10_ADDR_R 0x71

void initAht10 (void);
unsigned char readStatusBitAht10 (void);

void sendTriggerMeasurementCmd (void);

char readTemp (void);
