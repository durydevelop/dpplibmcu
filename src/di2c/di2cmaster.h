#ifndef DI2CMaster_H
#define DI2CMaster_H

#include <cstdint>
#include <string>
#include <di2c>

class DI2CMaster {
    public:
        DI2CMaster(DI2CBusHandle i2cBusHandle);
        ~DI2CMaster();

        bool isReady(void);
        std::string getLastError(void);

        bool sendByte(uint8_t slaveAddr, uint8_t data);

        uint16_t askForWord(uint8_t slaveAddr, uint8_t cmdReg);
        bool askForBuf(uint8_t slaveAddr, uint8_t cmdReg, uint8_t *recvBuf, const int recvLen);

        bool writeWord(uint8_t slaveAddr, uint8_t cmdReg, uint16_t data);

        uint16_t recvWord(uint8_t slaveAddr);
        

        //uint8_t i2c_read_byte(uint32_t i2c_master_port, uint8_t address, uint8_t command);
        //uint32_t i2c_write_byte(uint32_t i2c_master_port, uint8_t address, uint8_t command, uint8_t data);

        
        

        

        //uint32_t i2c_read_buf(uint32_t i2c_master_port, uint8_t address, uint8_t command, uint8_t *buffer, uint8_t len);
        //uint32_t i2c_write_buf(uint32_t i2c_master_port, uint8_t address, uint8_t command, uint8_t *data, uint8_t len);

    private:
        bool ask(uint8_t slaveAddr, uint8_t *sendBuf, const int sendLen, uint8_t *recvBuf, const int recvLen);

        DI2CBusHandle busHandle;
        std::string lastErrorString;

        
};

#endif