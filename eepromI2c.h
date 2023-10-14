#define I2C_EEPROM_ADDR_W 0xA0
#define I2C_EEPROM_ADDR_R 0xA1

#define EEPROM_256K

#ifdef EEPROM_256K
    //  pagina 1 reservada(32bytes) + 1354*3=4062 = 4094 sobra 2 bytes
    #define MAX_NUM_OF_LOG_EVENT 10912 
#endif

#ifdef EEPROM_128K
    //  pagina 1 reservada(32bytes) + 1354*3=4062 = 4094 sobra 2 bytes
    #define MAX_NUM_OF_LOG_EVENT 5450
#endif

#ifdef EEPROM_64K
    //  pagina 1 reservada(32bytes) + 1354*3=4062 = 4094 sobra 2 bytes
    #define MAX_NUM_OF_LOG_EVENT 2720
#endif

#ifdef EEPROM_32K
    //  pagina 1 reservada(32bytes) + 1354*3=4062 = 4094 sobra 2 bytes
    #define MAX_NUM_OF_LOG_EVENT 1354
#endif

unsigned char eepromReadByte(unsigned int addr);
void eepromWriteByte(unsigned int addr, unsigned char data);
void eepromReadLog(unsigned int addr, unsigned char* buff);
void eepromReadLogPage(unsigned int addr, unsigned char* buff);