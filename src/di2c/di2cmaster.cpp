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
#include "di2cmaster.h"

#define LOWORD(l) ((unsigned short)(l))
#define HIWORD(l) ((unsigned short)(((unsigned int)(l) >> 16) & 0xFFFF))
#define LOBYTE(w) (w & 0x00FF)
//#define HIBYTE(w) (data & 0xFF00) >> 8
#define HIBYTE(w) ((w >> 8) & 0x00FF)
#define WORD(msb,lsb) ((msb << 8) | lsb)
#define DWORD(msw,lsw) ((msw << 16) | lsw)
#define DWORD_B(msb,next_msb,next_lsb,lsb) (msb << 24) | (next_msb << 16) | (next_lsb << 8) | lsb

DI2CMaster::DI2CMaster(DI2CBusHandle i2cBusHandle)
{
    busHandle=i2cBusHandle;
}

DI2CMaster::~DI2CMaster()
{
}

bool DI2CMaster::isReady(void)
{
    return busHandle >= 0;
}

std::string DI2CMaster::getLastError(void)
{
    return lastErrorString;
}

bool DI2CMaster::ask(uint8_t slaveAddr, uint8_t *sendBuf, const int sendLen, uint8_t *recvBuf, const int recvLen)
{
    // Compose msgs packet
    i2c_msg msgs[2] = {
        {
            .addr = static_cast<uint16_t>(slaveAddr),
            .flags = {},
            .len = static_cast<uint16_t>(sendLen),
            .buf = sendBuf
        },
        {
            .addr = static_cast<uint16_t>(slaveAddr),
            .flags = I2C_M_RD,
            .len = static_cast<uint16_t>(recvLen),
            .buf = recvBuf
        }
    };
    i2c_rdwr_ioctl_data wrapper = {
        .msgs = msgs,
        .nmsgs = 2
    };

    // Perform I/O
    int ret=ioctl(busHandle, I2C_RDWR, &wrapper);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    if (msgs[0].len != sendLen || msgs[1].len != recvLen) {
        lastErrorString="msgs len missing real size";
        return false;
    }

    return true;
}

bool DI2CMaster::writeWord(uint8_t slaveAddr, uint8_t cmdReg, uint16_t data)
{
    // Create send buffer
    uint8_t sendBuf[3];
    sendBuf[0]=cmdReg;
    sendBuf[1]=HIBYTE(data);   // HIBYTE
    sendBuf[2]=LOBYTE(data);   // LOWBYTE

    // Compose msgs packet (only one)
    i2c_msg msgs[1] = {
        {
            .addr = static_cast<uint16_t>(slaveAddr),
            .flags = {},
            .len = sizeof(sendBuf), // 3
            .buf = sendBuf
        }
    };
    i2c_rdwr_ioctl_data dataWapped = {
        .msgs = msgs,
        .nmsgs = 1
    };

    // Perform I/O
    int ret=ioctl(busHandle, I2C_RDWR, &dataWapped);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    if (msgs[0].len != sizeof(sendBuf)) {
        lastErrorString="msgs len missing real size";
        return false;
    }

    return true;
}

uint16_t DI2CMaster::recvWord(uint8_t slaveAddr)
{
    // Create recv buffer
    uint8_t recvWord[2]={0,0};

    // Compose msgs packet (only one)
    i2c_msg msgs[1] = {
        {
            .addr = static_cast<uint16_t>(slaveAddr),
            .flags = I2C_M_RD,
            .len = 2,
            .buf = recvWord
        }
    };
    i2c_rdwr_ioctl_data wrapper = {
        .msgs = msgs,
        .nmsgs = 1
    };

    // Perform I/O
    int ret=ioctl(busHandle, I2C_RDWR, &wrapper);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return 0XFF;
    }

    if (msgs[0].len != 2) {
        lastErrorString="msgs len missing real size";
        return 0xFFFF;
    }

    return WORD(recvWord[0],recvWord[1]);
}

uint16_t DI2CMaster::askForWord(uint8_t slaveAddr, uint8_t cmdReg)
{
    uint8_t recvWord[2];
    ask(slaveAddr,&cmdReg,1,recvWord,2);
    //printf("recvWord: %02X %02X\r\n",recvWord[0],recvWord[1]);
    return WORD(recvWord[0],recvWord[1]);

}

bool DI2CMaster::askForBuf(uint8_t slaveAddr, uint8_t cmdReg, uint8_t *recvBuf, const int recvLen)
{
    return ask(slaveAddr,&cmdReg,1,recvBuf,recvLen);
}

bool DI2CMaster::sendByte(uint8_t slaveAddr, uint8_t data)
{
    // Compose msgs packet
    i2c_msg msgs[1] = {
        {
            .addr = static_cast<uint16_t>(slaveAddr),
            .flags = {},
            .len = 1,
            .buf = &data
        }
    };
    i2c_rdwr_ioctl_data wrapper = {
        .msgs = msgs,
        .nmsgs = 1
    };

    // Perform I/O
    int ret=ioctl(busHandle, I2C_RDWR, &wrapper);
    lastErrorString=strerror(errno);

    if (ret < 0) {
        return false;
    }

    if (msgs[0].len != 1) {
        lastErrorString="msgs len missing real size";
        return false;
    }

    return true;
}
