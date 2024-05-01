#ifndef DI2CMaster_H
#define DI2CMaster_H

#include <cstdint>
#include <string>
#include <di2c>

class DI2CMaster {
    public:
        DI2CMaster(DI2CBusHandle i2cBusHandle, uint16_t i2cMaxBufferLenght = 0);
        ~DI2CMaster();

        bool isReady(void);
        std::string getLastError(void);

        // Write commands (aka registers) methods
        bool writeByte(uint8_t slaveAddr, uint8_t cmdReg, uint8_t data);
        bool writeWord(uint8_t slaveAddr, uint8_t cmdReg, uint16_t data);
        bool writeBuf(uint8_t slaveAddr, uint8_t cmdReg, uint8_t *writeBuffer, uint16_t writeLen);
        
        // Read commands (aka registers) methods
        uint8_t recvByte(uint8_t slaveAddr);
        uint16_t recvWord(uint8_t slaveAddr);
        bool recvBuf(uint8_t slaveAddr, uint8_t *recvBuf, uint16_t recvLen);

        // Ask (write + read) commands (aka registers) methods
        uint8_t askForByte(uint8_t slaveAddr, uint8_t cmdReg);
        uint16_t askForWord(uint8_t slaveAddr, uint8_t cmdReg);
        bool askForBuf(uint8_t slaveAddr, uint8_t cmdReg, uint8_t *recvBuf, uint16_t recvLen);

        // Raw send methods
        bool sendByte(uint8_t slaveAddr, uint8_t data);
        bool sendWord(uint8_t slaveAddr, uint16_t data);
        bool sendBuf(uint8_t slaveAddr, uint8_t *data, uint16_t dataLen);
        
    private:
        DI2CBusHandle busHandle;
        std::string lastErrorString;
        size_t maxBufLenght;
        
};

#endif