/**
 * @file di2cmaster.cpp
 * @author your name (you@domain.com)
 * @brief Class to handle i2c master comunication node
 * 
 * There are three type of API for sending data:
 * 1 - Methods that starts with write...() like writeByte(), writeBuf(), etc; these methods needs a command (aka register) and data. usually used to comunicate with sensor boards.
 * 2 - Methods that starts with AskFor...() like askForByte(), askForBuf(), etc; these methods are same as write...() ones but after send command and data, wait for data back from slave.
 * 3 - Methods that start with send...() like sendByte(), sendBuf(), etc; these methods does not need command: the buffer is sent arbitrary as passed. Usually used for data comunication though devices, like rpi to arduino, arduino to arduino, etc.
 * 
 * N.B.
 * Arduino Wire library set limit of each i2c transaction to 32 bytes, in this case, setting i2cMaxBufferLenght to 32, the class handle it by it self:
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

#define LOWORD(l) ((unsigned short)(l))
#define HIWORD(l) ((unsigned short)(((unsigned int)(l) >> 16) & 0xFFFF))
#define LOBYTE(w) ((uint8_t)(w&0x00FF))
#define HIBYTE(w) ((uint8_t)((w>>8)&0x00FF))
#define WORD(msb,lsb) ((msb << 8) | lsb)
#define DWORD(msw,lsw) ((msw << 16) | lsw)
#define DWORD_B(msb,next_msb,next_lsb,lsb) (msb << 24) | (next_msb << 16) | (next_lsb << 8) | lsb

/**
 * @brief Construct a new DI2CMaster::DI2CMaster object
 * 
 * @param i2cBusHandle          ->  an handle for the i2c bus (can obtained by DI2CBus() class).
 * @param i2cMaxBufferLenght    ->  max lenght of tx buffer used in each transaction.
 *                                  - If value is 0, no limit are set.
 *                                  - If value is greater than 0, write...() and askFor...() API truncate buffer of each transaction to this value, send...() API do as many transaction as need to send all data.
 */
DI2CMaster::DI2CMaster(DI2CBusHandle i2cBusHandle, uint16_t i2cMaxBufferLenght)
{
    busHandle=i2cBusHandle;
    maxBufLenght=i2cMaxBufferLenght;
    if (busHandle >= 0) {
        lastErrorString="Success";
    }
    else {
        lastErrorString="Bus handle not valid";
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
    return lastErrorString;
}

/**
 * @brief Write a BYTE at specified i2c register of the slave device.
 * ...that means "send 1 command byte followed by 1 data byte"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @param data      -> the byte to send.
 * @return true on success, otherwise false (you can retrive the error by calling getLastError()).
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
    int ret=ioctl(busHandle, I2C_RDWR, &ioctlData);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    if (message.len != 2) {
        lastErrorString="msgs len missing real size";
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
 * @return true on success, otherwise false (you can retrive the error by calling getLastError()).
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
    int ret=ioctl(busHandle, I2C_RDWR, ioctlData);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    if (message.len != 3) {
        lastErrorString="msgs len missing real size";
        return false;
    }

    return true;
}

/**
 * @brief Write a BUFFER at specified i2c register of the slave device.
 * ...that means "send 1 command byte followed by N data bytes"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @param data      -> the buffer to send.
 * @param dataLen   -> the lenght of the buffer.
 * @return true on success, otherwise false (you can retrive the error by calling getLastError()).
 */
 bool DI2CMaster::writeBuf(uint8_t slaveAddr, uint8_t cmdReg, uint8_t *data, uint16_t dataLen)
{
    if (maxBufLenght > 0) {
        if (dataLen > maxBufLenght) {
            // Truncate over
            dataLen=maxBufLenght;
        }
    }

    // Compose i2c message (1 + dataLen bytes)
    uint16_t realLen=dataLen+1;
    uint8_t sendBuf[realLen]={cmdReg};
    memcpy(&sendBuf[1],data,dataLen);
    
    struct i2c_msg message={slaveAddr, 0, realLen, sendBuf};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    int ret=ioctl(busHandle, I2C_RDWR, ioctlData);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    if (message.len != realLen) {
        lastErrorString="msgs len missing real size";
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
    ioctl(busHandle, I2C_RDWR, &ioctlData);
    lastErrorString=strerror(errno);

    if (message.len != 1) {
        lastErrorString="WARNING: msgs len missing real size";
    }

    //printf("recvWord: %02X %02X\r\n",recvWord[0],recvWord[1]);
    return recvByte;
}

/**
 * @brief Read a WORD(2 bytes) from an i2c slave device.
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
    ioctl(busHandle, I2C_RDWR, &ioctlData);
    lastErrorString=strerror(errno);

    if (message.len != 2) {
        lastErrorString="WARNING: msgs len missing real size";
    }

    //printf("recvWord: %02X %02X\r\n",recvWord[0],recvWord[1]);
    return WORD(recvWord[0],recvWord[1]);
}

/**
 * @brief Read a BUFFER from an i2c slave device.
 * 
 * @param slaveAddr     -> i2c slave device address.
 * @param recvBuffer    -> pointer to a buffer for received data.
 * @param recvLen       -> lenght of data to read.
 * @return true on success, otherwise false (you can retrive the error by calling getLastError()).
 */
 bool DI2CMaster::recvBuf(uint8_t slaveAddr, uint8_t *recvBuffer, uint16_t recvLen)
{
    if (maxBufLenght > 0) {
        if (recvLen > maxBufLenght) {
            // Truncate to max lenght
            recvLen=maxBufLenght;
        }
    }

    // Compose i2c message
    struct i2c_msg message = {slaveAddr, I2C_M_RD, recvLen, recvBuffer};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    int ret=ioctl(busHandle, I2C_RDWR, &ioctlData);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    if (message.len != recvLen) {
        lastErrorString="msgs len missing real size";
        return false;
    }

    //printf("recvWord: %02X %02X\r\n",recvWord[0],recvWord[1]);
    return true;
}

/**
 * @brief Read a BYTE from specified i2c register of the slave device.
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
    ioctl(busHandle, I2C_RDWR, &ioctlData);
    lastErrorString=strerror(errno);

    if (messages[0].len != 1 || messages[1].len != 1) {
        lastErrorString="WARNING: msgs len missing real size";
    }

    //printf("recvWord: %02X %02X\r\n",recvWord[0],recvWord[1]);
    return recvByte;
}

/**
 * @brief Read a WORD from specified i2c register of the slave device.
 * ...that means "send 1 command byte and read 2 bytes"...
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param cmdReg    -> i2c device command (aka register).
 * @return WORD value read. If returned vaue is 0xFFFF can be that something was wrong.
 */
uint16_t DI2CMaster::askForWord(uint8_t slaveAddr, uint8_t cmdReg)
{
    // Create recv buffer (2 bytes)
    uint8_t recvWord[2]={ 0xFF, 0xFF };

    // Compose i2c messages array
    struct i2c_msg messages[]={
        {slaveAddr, 0, 1,  &cmdReg},
        {slaveAddr, I2C_M_RD | I2C_M_NOSTART, 2, recvWord},
    };
    struct i2c_rdwr_ioctl_data ioctlData = {messages, 2};

    // Perform I/O
    ioctl(busHandle, I2C_RDWR, &ioctlData);
    lastErrorString=strerror(errno);

    if (messages[0].len != 1 || messages[1].len != 2) {
        lastErrorString="WARNING: msgs len missing real size";
    }

    printf("recvWord: %02X %02X\r\n",recvWord[0],recvWord[1]);
    return WORD(recvWord[0],recvWord[1]);
}

/**
 * @brief Read a WORD from specified i2c register of the slave device.
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
    ioctl(busHandle, I2C_RDWR, &ioctlData);
    lastErrorString=strerror(errno);

    if (messages[0].len != 1 || messages[1].len != 4) {
        lastErrorString="WARNING: msgs len missing real size";
    }

    //printf("recvWord: %02X %02X\r\n",recvDWord[0],recvDWord[1]);
    return DWORD_B(recvDWord[0],recvDWord[1],recvDWord[2],recvDWord[3]);
}

/**
 * @brief Read a BUFFER from specified i2c register of the slave device.
 * ...that means "send 1 command byte and read N bytes"...
 * 
 * @param slaveAddr     -> i2c slave device address.
 * @param cmdReg        -> i2c device command (aka register).
 * @param recvBuffer    -> pointer to a buffer for received data.
 * @param recvLen       -> lenght of data to read.
 * @return true on success, otherwise false (you can retrive the error by calling getLastError()).
 */
bool DI2CMaster::askForBuf(uint8_t slaveAddr, uint8_t cmdReg, uint8_t *recvBuffer, uint16_t recvLen)
{
    if (maxBufLenght > 0) {
        if (recvLen > maxBufLenght) {
            // Limit receive lenght
            recvLen=maxBufLenght;
        }
    }

    // Create i2c messages array
    struct i2c_msg messages[]={
        {slaveAddr, 0, 1,  &cmdReg},
        {slaveAddr, I2C_M_RD | I2C_M_NOSTART, recvLen, recvBuffer},
    };
    struct i2c_rdwr_ioctl_data ioctlData = {messages, 2};

    // Perform I/O
    int ret=ioctl(busHandle, I2C_RDWR, &ioctlData);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    if (messages[0].len != 1 || messages[1].len != recvLen) {
        lastErrorString="WARNING: msgs len missing real size";
    }
    
    return true;
}

/**
 * @brief Send a BYTE to a slave device.
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param data      -> the byte to send.
 * @return true on success, otherwise false (you can retrive the error by calling getLastError()).
 */
bool DI2CMaster::sendByte(uint8_t slaveAddr, uint8_t data)
{
    // Create i2c message
    i2c_msg message={slaveAddr, 0, 1, &data};
    struct i2c_rdwr_ioctl_data ioctlData={&message, 1};

    // Perform I/O
    int ret=ioctl(busHandle, I2C_RDWR, &ioctlData);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    if (message.len != 1) {
        lastErrorString="WARNING: msgs len missing real size";
    }

    return true;
}

/**
 * @brief Send a WORD (2 bytes) to a slave device.
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param data      -> the word to send.
 * @return true on success, otherwise false (you can retrive the error by calling getLastError()).
 */
bool DI2CMaster::sendWord(uint8_t slaveAddr, uint16_t data)
{
    // Create send buffer
    uint8_t sendWord[2]={HIBYTE(data), LOBYTE(data)};

    // Create i2c message
    i2c_msg message={slaveAddr, 0, 2, sendWord};
    struct i2c_rdwr_ioctl_data ioctData={&message, 1};

    // Perform I/O
    int ret=ioctl(busHandle, I2C_RDWR, &ioctData);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    if (message.len != 2) {
        lastErrorString="WARNING: msgs len missing real size";
    }

    return true;
}

/**
 * @brief Send a BUFFER to a slave device.
 * 
 * @param slaveAddr -> i2c slave device address.
 * @param data      -> buffer to send.
 * @param dataLen   -> the lenght of the buffer.
 * @return true on success, otherwise false (you can retrive the error by calling getLastError()).
 */
bool DI2CMaster::sendBuf(uint8_t slaveAddr, uint8_t *data, uint16_t dataLen)
{
    printf("datalen=%d\r\n",dataLen);
    for (int i=0; i<dataLen; i++) printf("%c",data[i]);printf("\r\n");

    // Default: only one transaction
    uint16_t msgCount=1;
    uint16_t msgLen=dataLen;
    uint16_t lastMsgLen;

    if (maxBufLenght > 0) {
        // 1 transaction each maxBufLenght bytes
        msgCount=dataLen/maxBufLenght;
        msgLen=maxBufLenght;
        lastMsgLen=dataLen%maxBufLenght;
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
        printf("flags=%d\r\n",flags);
        messages[ixMsg]={slaveAddr, 0, msgLen, txData};
        txData+=msgLen;
    }
    struct i2c_rdwr_ioctl_data ioctData={messages, msgCount};

    // Perform I/O
    int ret=ioctl(busHandle, I2C_RDWR, &ioctData);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    return true;
}
