/**
 * @file di2cmaster.cpp
 * @author your name (you@domain.com)
 * @brief Class to handle i2c master communication node
 * 
 * There are three type of API for sending data:
 * 1 - Methods that starts with write...() like writeByte(), writeBuf(), etc; these methods needs a command (aka register) and data. usually used to comunicate with sensor boards.
 * 2 - Methods that starts with AskFor...() like askForByte(), askForBuf(), etc; these methods are same as write...() ones but after send command and data, wait for data back from slave.
 * 3 - Methods that start with send...() like sendByte(), sendBuf(), etc; these methods does not need command: the buffer is sent arbitrary as passed. Usually used for data communication though devices, like rpi to arduino, arduino to arduino, etc.
 * 
 * N.B.
 * Arduino Wire library set limit of each i2c transaction to 32 bytes, in this case, setting i2cMaxBufferLength to 32, the class handle it by it self:
 * - If value is 0, no limit are set.
 * - If value is greater than 0, write...() and askFor...() API truncate buffer of each transaction to this value (like arduino do), send...() API do as many transaction as need to send all data.
 * 
 * How to use:
 *
 * @code
 * DI2CBus i2cBus(busID);
 * DI2CMaster attiny(i2cBus.handle());
 * @endcode
 * 
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 * TODO:
 *  _se fallisce iotcl provare con { slaveAddr, I2C_M_RD, recvLen, recvBuffer }
 *  _Eliminare if (message.len)
 */

#include "di2cmaster.h"
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <byteswap.h>
#include <iostream>
#include <map>
#include <cstring>

#define LOWORD(l) ((unsigned short)(l))
#define HIWORD(l) ((unsigned short)(((unsigned int)(l) >> 16) & 0xFFFF))
#define LOBYTE(w) ((uint8_t)(w&0x00FF))
#define HIBYTE(w) ((uint8_t)((w>>8)&0x00FF))
#define WORD(msb,lsb) ((msb << 8) | lsb)
#define DWORD(msw,lsw) ((msw << 16) | lsw)
#define DWORD_B(msb,next_msb,next_lsb,lsb) (msb << 24) | (next_msb << 16) | (next_lsb << 8) | lsb

#define ERR_TXT_SUCCESS "Success"
#define ERR_BUS_HANDLE_NOT_VALID "Bus handle not valid"


/**
 * @brief Construct a new DI2CMaster::DI2CMaster object
 * 
 * @param i2cBusHandle          ->  an handle for the i2c bus (can obtained by DI2CBus() class).
 * @param i2cMaxBufferLength    ->  max length of tx buffer used in each transaction.
 *                                  - If value is 0, no limit are set.
 *                                  - If value is greater than 0, write...() and askFor...() API truncate buffer of each transaction to this value, send...() API do as many transaction as need to send all data.
 */
DI2CMaster::DI2CMaster(DI2CBusHandle i2cBusHandle, uint16_t i2cMaxBufferLength)
{
    busHandle=i2cBusHandle;
    maxBufLength=i2cMaxBufferLength;
    if (busHandle >= 0) {
        lastErrorString=ERR_TXT_SUCCESS;
    }
    else {
        lastErrorString=ERR_BUS_HANDLE_NOT_VALID ": " + std::to_string(i2cBusHandle);
    }
}

DI2CMaster::~DI2CMaster()
{
}

/**
 * @return true if there is a bus handle valid.
 */
bool DI2CMaster::isReady(void)
{
    return busHandle >= 0;
}

/**
 * @return last result of io operation.
 */
std::string DI2CMaster::getLastError(void)
{
    return lastErrorString.empty() ? ERR_TXT_SUCCESS : lastErrorString;
}

/**
 * @brief Write a BYTE at specified i2c register of the slave device.
 * ...that means "send 1 command byte followed by 1 data byte"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @param data      -> the byte to send.
 * @return true on success, otherwise false (you can retrieve the error by calling getLastError()).
 */
bool DI2CMaster::writeByte(uint8_t slaveAddr, uint8_t cmdReg, uint8_t data)
{
    // Compose i2c message (2 bytes)
    uint8_t sendBuf[2]={
        cmdReg, // 1 byte command
        data    // 1 byte data
    };
    struct i2c_msg message={slaveAddr, 0, 2, sendBuf};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return false;
    }

    return true;
}

/**
 * @brief Write a WORD (2 bytes) at specified i2c register of the slave device.
 * ...that means "send 1 command byte followed by 2 data bytes"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @param data      -> the WORD to send.
 * @return true on success, otherwise false (you can retrieve the error by calling getLastError()).
 */
bool DI2CMaster::writeWord(uint8_t slaveAddr, uint8_t cmdReg, uint16_t data)
{
    // Compose i2c message ( 3 bytes)
    uint8_t sendBuf[3]={
        cmdReg,                     // 1 byte command (aka register)
        HIBYTE(data), LOBYTE(data)  // 2 bytes data
    };
    struct i2c_msg message={slaveAddr, 0, 3, sendBuf};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return false;
    }

    return true;
}

/**
 * @brief Write a DWORD (4 bytes) at specified i2c register of the slave device.
 * ...that means "send 1 command byte followed by 4 data bytes"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @param data      -> the DWORD to send.
 * @return true on success, otherwise false (you can retrieve the error by calling getLastError()).
 */
bool DI2CMaster::writeDWord(uint8_t slaveAddr, uint8_t cmdReg, uint32_t data)
{
    // Compose i2c message ( 5 bytes)
    uint8_t sendBuf[5]={
        cmdReg,                         // 1 byte command (aka register)
        /*
        HIWORD(HIBYTE(data)),
        HIWORD(LOBYTE(data)),
        LOWORD(HIBYTE(data)),
        LOWORD(LOBYTE(data))            // 4 bytes data
        */
        // Più veloce
        (uint8_t)(data >> 24),          // 4 bytes data
        (uint8_t)(data >> 16),
        (uint8_t)(data >> 8),
        (uint8_t)(data & 0x000000FF)
    };
    struct i2c_msg message={slaveAddr, 0, 5, sendBuf};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return false;
    }

    return true;
}

/**
 * @brief Write a FLOAT (4 bytes) at specified i2c register of the slave device.
 * ...that means "send 1 command byte followed by 4 data bytes"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @param data      -> the FLOAT to send.
 * @return true on success, otherwise false (you can retrieve the error by calling getLastError()).
 */
bool DI2CMaster::writeFloat(uint8_t slaveAddr, uint8_t cmdReg, float data)
{
    union {
        float ff;
        uint32_t ii;
    }d;
    d.ff=data;
    return writeDWord(slaveAddr,cmdReg,d.ii);
}

/**
 * @brief Write a BUFFER at specified i2c register of the slave device.
 * ...that means "send 1 command byte followed by N data bytes"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @param data      -> the buffer to send.
 * @param dataLen   -> the length of the buffer.
 * @return true on success, otherwise false (you can retrieve the error by calling getLastError()).
 */
 bool DI2CMaster::writeBuf(uint8_t slaveAddr, uint8_t cmdReg, uint8_t *data, uint16_t dataLen)
{
    if (maxBufLength > 0) {
        if (dataLen > maxBufLength) {
            // Truncate over
            dataLen=maxBufLength;
        }
    }

    // Compose i2c message (1 + dataLen bytes)
    uint16_t realLen=dataLen+1;
    //uint8_t sendBuf[realLen]={cmdReg};
    // C++ compliant
    std::vector<uint8_t> sendBuf(realLen);
    sendBuf[0] = cmdReg;
    memcpy(&sendBuf[1],data,dataLen);
    
    //struct i2c_msg message={slaveAddr, 0, realLen, sendBuf};
    struct i2c_msg message={slaveAddr, 0, realLen, sendBuf.data()};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return false;
    }

    return true;
}

/**
 * @brief Read a BYTE from an i2c slave device.
 * 
 * @param slaveAddr -> i2c slave device address.
 * @return BYTE value read. If returned vaue is 0xFF can be that something was wrong.
 */
uint8_t DI2CMaster::recvByte(uint8_t slaveAddr)
{
    // Recv byte
    uint8_t recvByte=0xFF;

    // Compose i2c message
    struct i2c_msg message={slaveAddr, I2C_M_RD, 1, &recvByte};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return 0xFF;
    }

    //printf("recvWord: %02X %02X\r\n",recvWord[0],recvWord[1]);
    return recvByte;
}

/**
 * @brief Read a WORD (2 bytes) from an i2c slave device.
 * 
 * @param slaveAddr -> i2c slave device address.
 * @return WORD value read. If returned value is 0xFFFF can be that something was wrong.
 */
uint16_t DI2CMaster::recvWord(uint8_t slaveAddr)
{
    // Create recv buffer (2 bytes)
    uint8_t recvWord[2]={ 0xFF, 0xFF };

    // Compose i2c message
    struct i2c_msg message={slaveAddr, I2C_M_RD, 2, recvWord};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return 0xFFFF;
    }

    //printf("recvWord: %02X %02X\r\n",recvWord[0],recvWord[1]);
    return WORD(recvWord[0],recvWord[1]);
}

/**
 * @brief Read a DWORD (4 bytes) from an i2c slave device.
 * 
 * @param slaveAddr -> i2c slave device address.
 * @return WORD value read. If returned value is 0xFFFFFFFF can be that something was wrong.
 */
uint32_t DI2CMaster::recvDWord(uint8_t slaveAddr)
{
    // Create recv buffer (4 bytes)
    uint8_t recvDWord[4]={ 0xFF, 0xFF, 0xFF, 0xFF };

    // Compose i2c message
    struct i2c_msg message={slaveAddr, I2C_M_RD, 4, recvDWord};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return 0xFFFFFFFF;
    }

    //printf("recvWord: %02X %02X %02X %02X\r\n",recvDWord[0],recvDWord[1],recvDWord[2],recvDWord[3]);
    return DWORD_B(recvDWord[0],recvDWord[1],recvDWord[2],recvDWord[3]);
}

/**
 * @brief Read a FLOAT (4 bytes) from an i2c slave device.
 * 
 * @param slaveAddr -> i2c slave device address.
 * @return WORD value read. If returned value is 0xFFFFFFFF can be that something was wrong.
 */
float DI2CMaster::recvFloat(uint8_t slaveAddr)
{
    union {
        float ff;
        uint32_t ii;
    }d;
    d.ii=recvDWord(slaveAddr);
    return d.ff;
}

/**
 * @brief Read a BUFFER from an i2c slave device.
 * 
 * @param slaveAddr     -> i2c slave device address.
 * @param recvBuffer    -> pointer to a buffer for received data.
 * @param recvLen       -> length of data to read.
 * @return true on success, otherwise false (you can retrieve the error by calling getLastError()).
 */
 bool DI2CMaster::recvBuf(uint8_t slaveAddr, uint8_t *recvBuffer, uint16_t recvLen)
{
    if (maxBufLength > 0) {
        if (recvLen > maxBufLength) {
            // Truncate to max length
            recvLen=maxBufLength;
        }
    }

    // Compose i2c message
    struct i2c_msg message = {slaveAddr, I2C_M_RD, recvLen, recvBuffer};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return false;
    }

    //printf("recvWord: %02X %02X\r\n",recvWord[0],recvWord[1]);
    return true;
}

/**
 * @brief Read a BYTE from specified i2c register of the slave device (aka read register).
 * ...that means "send 1 command byte and read 1 byte"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @return BYTE value read. If returned vaue is 0xFF can be that something was wrong.
 */
uint8_t DI2CMaster::askForByte(uint8_t slaveAddr, uint8_t cmdReg)
{
    // Recv byte
    uint8_t recvByte=0xFF;

    // Compose i2c messages array
    struct i2c_msg messages[]={
        {slaveAddr, 0, 1,  &cmdReg},
        {slaveAddr, I2C_M_RD | I2C_M_NOSTART, 1, &recvByte},
    };
    struct i2c_rdwr_ioctl_data ioctlData = {messages, 2};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return 0xFF;
    }

    //printf("recvWord: %02X %02X\r\n",recvWord[0],recvWord[1]);
    return recvByte;
}

/**
 * @brief Read a WORD from specified i2c register of the slave device (aka read register).
 * ...that means "send 1 command byte and read 2 bytes"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @return WORD value read. If returned vaue is 0xFFFF can be that something was wrong.
 */
uint16_t DI2CMaster::askForWord(uint8_t slaveAddr, uint8_t cmdReg)
{
    // Create recv buffer (2 bytes)
    uint8_t recvWord[2] = { 0xFF, 0xFF };

    // Compose i2c messages array (write + read)
    struct i2c_msg messages[] = {
        { slaveAddr, 0,        1, &cmdReg },
        { slaveAddr, I2C_M_RD, 2, recvWord }
    };

    struct i2c_rdwr_ioctl_data ioctlData = { messages, 2 };

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return 0xFFFF;
    }

    return WORD(recvWord[0], recvWord[1]);
}

/**
 * @brief Read a WORD from specified i2c register of the slave device (aka read register).
 * ...that means "send 1 command byte and read 2 bytes"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @return WORD value read. If returned vaue is 0xFFFF can be that something was wrong.
 */
uint32_t DI2CMaster::askForDWord(uint8_t slaveAddr, uint8_t cmdReg)
{
    // Create recv buffer (4 bytes)
    uint8_t recvDWord[4]={ 0xFF, 0xFF, 0xFF, 0xFF };

    // Compose i2c messages array
    struct i2c_msg messages[]={
        {slaveAddr, 0, 1,  &cmdReg},
        {slaveAddr, I2C_M_RD | I2C_M_NOSTART, 4, recvDWord},
    };
    struct i2c_rdwr_ioctl_data ioctlData = {messages, 2};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return 0xFFFFFFFF;
    }

    //printf("recvWord: %02X %02X %02X %02X\r\n",recvDWord[0],recvDWord[1],recvDWord[2],recvDWord[3]);
    return DWORD_B(recvDWord[0],recvDWord[1],recvDWord[2],recvDWord[3]);
}

float DI2CMaster::askForFloat(uint8_t slaveAddr, uint8_t cmdReg)
{
    uint32_t dword=askForDWord(slaveAddr,cmdReg);
    // To float
    float f;
    std::memcpy(&f, &dword, sizeof(f));

    return f;
}

int16_t DI2CMaster::askForInt16(uint8_t slaveAddr, uint8_t cmdReg)
{
    return static_cast<int16_t>(askForWord(slaveAddr,cmdReg));
}

/**
 * @brief Read a BUFFER from specified i2c register of the slave device (aka read register).
 * ...that means "send 1 command byte and read N bytes"...
 * 
 * @param slaveAddr     -> i2c slave device address.
 * @param cmdReg        -> i2c device command (aka register).
 * @param recvBuffer    -> pointer to a buffer for received data.
 * @param recvLen       -> length of data to read.
 * @return true on success, otherwise false (you can retrieve the error by calling getLastError()).
 */
bool DI2CMaster::askForBuf(uint8_t slaveAddr, uint8_t cmdReg, uint8_t *recvBuffer, uint16_t recvLen)
{
    if (maxBufLength > 0) {
        if (recvLen > maxBufLength) {
            // Limit receive length
            recvLen=maxBufLength;
        }
    }

    // Create i2c messages array
    struct i2c_msg messages[]={
        {slaveAddr, 0, 1,  &cmdReg},
        {slaveAddr, I2C_M_RD | I2C_M_NOSTART, recvLen, recvBuffer},
    };
    struct i2c_rdwr_ioctl_data ioctlData = {messages, 2};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return false;
    }
    
    return true;
}

/**
 * @brief Read a STRING from specified i2c register of the slave device (aka read register).
 * ...that means "send 1 command byte and read N bytes"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @return std::string value read. If returned string is empty can be that something was wrong.
 */
std::string DI2CMaster::askForString(uint8_t slaveAddr, uint8_t cmdReg)
{
    // Create recv buffer (max 255 bytes)
    uint8_t recvBuf[255] = { 0 };

    // Compose i2c messages array
    struct i2c_msg messages[]={
        {slaveAddr, 0, 1, &cmdReg},
        {slaveAddr, I2C_M_RD | I2C_M_NOSTART, 255, recvBuf},
    };
    struct i2c_rdwr_ioctl_data ioctlData = {messages, 2};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return "";
    }

    // Convert to std::string
    std::string result(reinterpret_cast<char*>(recvBuf), messages[1].len);

    // Remove trailing null characters
    result.erase(result.find_last_not_of('\0') + 1);
    return result;
}

/**
 * @brief Send a BYTE to a slave device.
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param data      -> the byte to send.
 * @return true on success, otherwise false (you can retrieve the error by calling getLastError()).
 */
bool DI2CMaster::sendByte(uint8_t slaveAddr, uint8_t data)
{
    // Create i2c message
    i2c_msg message={slaveAddr, 0, 1, &data};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return false;
    }

    return true;
}

/**
 * @brief Send a WORD (2 bytes) to a slave device.
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param data      -> the word to send.
 * @return true on success, otherwise false (you can retrieve the error by calling getLastError()).
 */
bool DI2CMaster::sendWord(uint8_t slaveAddr, uint16_t data)
{
    // Create send buffer
    uint8_t sendWord[2]={HIBYTE(data), LOBYTE(data)};

    // Create i2c message
    i2c_msg message={slaveAddr, 0, 2, sendWord};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return false;
    }

    return true;
}

/**
 * @brief Send a BUFFER to a slave device.
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param data      -> buffer to send.
 * @param dataLen   -> the length of the buffer.
 * @return true on success, otherwise false (you can retrieve the error by calling getLastError()).
 */
bool DI2CMaster::sendBuf(uint8_t slaveAddr, uint8_t *data, uint16_t dataLen)
{
    //printf("datalen=%d\r\n",dataLen);
    //for (int i=0; i<dataLen; i++) printf("%c",data[i]);printf("\r\n");

    // Default: only one transaction
    uint16_t msgCount=1;
    uint16_t msgLen=dataLen;
    uint16_t lastMsgLen;

    if (maxBufLength > 0) {
        // 1 transaction each maxBufLength bytes
        msgCount=dataLen/maxBufLength;
        msgLen=maxBufLength;
        lastMsgLen=dataLen%maxBufLength;
        if (lastMsgLen > 0) {
            msgCount++;
        }
    }

    // Create i2c message/s
    i2c_msg messages[msgCount];
    uint8_t *txData=data;
    uint16_t flags=0;
    for (size_t ixMsg=0; ixMsg<msgCount; ixMsg++) {
        if (ixMsg == msgCount-1) {
            msgLen=lastMsgLen;
        }
        if (ixMsg == 1) {
            flags=I2C_M_NOSTART;
        }
        //printf("flags=%d\r\n",flags);
        messages[ixMsg]={slaveAddr, 0, msgLen, txData};
        txData+=msgLen;
    }
    struct i2c_rdwr_ioctl_data ioctlData={messages, msgCount};

    // Perform I/O
    if (!performIoctl(busHandle, I2C_RDWR, &ioctlData)) {
        return false;
    }

    return true;
}

/*
bool DI2CMaster::checkIoctl(int ret, int expectedMsgs, std::string source)
{
    if (ret != expectedMsgs) {
        lastErrorString = strerror(errno);

        // opzionale: aggiungo contesto al messaggio di errore
        if (!source.empty()) {
            lastErrorString = source + ": " + lastErrorString;
        }

        return false;
    }
    return true;
}
*/

bool DI2CMaster::performIoctl(int fd, unsigned long int request, struct i2c_rdwr_ioctl_data* data)
{
    int ret = ioctl(fd, request, data);
    if (ret != data->nmsgs) {
        lastErrorString = strerror(errno);
        return false;
    }
    return true;
}

