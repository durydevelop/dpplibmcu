#ifndef DI2CMaster_H
#define DI2CMaster_H

#include <cstdint>
#include <string>
#include <di2c>

class DI2CMaster {
    public:
        DI2CMaster(DI2CBusHandle i2cBusHandle, uint16_t i2cMaxBufferLength = 0);
        ~DI2CMaster();

        bool isReady(void);
        std::string getLastError(void);

        // Write commands (aka registers) methods
        bool writeByte(uint8_t slaveAddr, uint8_t cmdReg, uint8_t data);
        bool writeWord(uint8_t slaveAddr, uint8_t cmdReg, uint16_t data);
        bool writeDWord(uint8_t slaveAddr, uint8_t cmdReg, uint32_t data);
        bool writeFloat(uint8_t slaveAddr, uint8_t cmdReg, float data);
        bool writeBuf(uint8_t slaveAddr, uint8_t cmdReg, uint8_t *writeBuffer, uint16_t writeLen);
        
        // Read commands (aka registers) methods
        uint8_t recvByte(uint8_t slaveAddr);
        uint16_t recvWord(uint8_t slaveAddr);
        uint32_t recvDWord(uint8_t slaveAddr);
        float recvFloat(uint8_t slaveAddr);
        bool recvBuf(uint8_t slaveAddr, uint8_t *recvBuf, uint16_t recvLen);

        // Ask (write + read) commands (aka registers) methods
        uint8_t askForByte(uint8_t slaveAddr, uint8_t cmdReg);
        uint16_t askForWord(uint8_t slaveAddr, uint8_t cmdReg);
        uint32_t askForDWord(uint8_t slaveAddr, uint8_t cmdReg);
        float askForFloat(uint8_t slaveAddr, uint8_t cmdReg);
        int16_t askForInt16(uint8_t slaveAddr, uint8_t cmdReg);
        bool askForBuf(uint8_t slaveAddr, uint8_t cmdReg, uint8_t *recvBuf, uint16_t recvLen);
        std::string askForString(uint8_t slaveAddr, uint8_t cmdReg);

        // Raw send methods
        bool sendByte(uint8_t slaveAddr, uint8_t data);
        bool sendWord(uint8_t slaveAddr, uint16_t data);
        bool sendBuf(uint8_t slaveAddr, uint8_t *data, uint16_t dataLen);
        
    private:
        //bool checkIoctl(int ret, int expectedMsgs, std::string source);
        bool performIoctl(int fd, unsigned long int request, struct i2c_rdwr_ioctl_data* data);

        DI2CBusHandle busHandle;
        size_t maxBufLength;

    protected:
        std::string lastErrorString;
};

#endif