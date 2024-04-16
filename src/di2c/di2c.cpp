#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <map>
#include <iostream>
#include "di2c.h"

static const std::map<ulong,std::string> I2CFuncList = {
	{ I2C_FUNC_I2C,                     "I2C                        " },
	{ I2C_FUNC_SMBUS_QUICK,             "SMBus Quick Command        " },
	{ I2C_FUNC_SMBUS_WRITE_BYTE,        "SMBus Send Byte            " },
	{ I2C_FUNC_SMBUS_READ_BYTE,         "SMBus Receive Byte         " },
	{ I2C_FUNC_SMBUS_WRITE_BYTE_DATA,   "SMBus Write Byte           " },
	{ I2C_FUNC_SMBUS_READ_BYTE_DATA,    "SMBus Read Byte            " },
	{ I2C_FUNC_SMBUS_WRITE_WORD_DATA,   "SMBus Write Word           " },
	{ I2C_FUNC_SMBUS_READ_WORD_DATA,    "SMBus Read Word            " },
	{ I2C_FUNC_SMBUS_PROC_CALL,         "SMBus Process Call         " },
	{ I2C_FUNC_SMBUS_WRITE_BLOCK_DATA,  "SMBus Block Write          " },
	{ I2C_FUNC_SMBUS_READ_BLOCK_DATA,   "SMBus Block Read           " },
	{ I2C_FUNC_SMBUS_BLOCK_PROC_CALL,   "SMBus Block Process Call   " },
	{ I2C_FUNC_SMBUS_PEC,               "SMBus PEC                  " },
	{ I2C_FUNC_SMBUS_WRITE_I2C_BLOCK,   "I2C Block Write            " },
	{ I2C_FUNC_SMBUS_READ_I2C_BLOCK,    "I2C Block Read             " },
};

#define I2C_BUS_PREFIX "/dev/i2c-"

DI2C::DI2C(uint8_t busID)
{
    busName=I2C_BUS_PREFIX+std::to_string(busID);

    busHandle = open(busName.c_str(), O_RDWR);
    lastResult=errno;
}

DI2C::~DI2C()
{
    if (close(busHandle) < 0) {
        std::cerr << "Closing " << busName << " error: " << strerror(errno) << std::endl;
    }
}

DI2CBusHandle DI2C::handle(void)
{
    return busHandle;
}

bool DI2C::isReady(void)
{
    return busHandle >= 0;
}

std::string DI2C::getLastError(void)
{
    return strerror(lastResult);
}

std::string DI2C::getInfo(void)
{
    ulong funcs;
    int ret=ioctl(busHandle, I2C_FUNCS, &funcs);
    lastResult=errno;

    if (ret < 0) {
        return getLastError();
    }

    std::string resultList;
    for (auto [value,name] : I2CFuncList) {
        std::string res=(funcs & value) ? "yes" : "no";
        resultList+=name+res+"\n";
	}
    return resultList;
}
/*
bool DI2C::begin(uint8_t deviceAddress)
{
    int ret=ioctl(busHandle, I2C_SLAVE, deviceAddress);
    std::cerr << "begin() ioctl ret=" << ret << std::endl;
    lastErrorString=strerror(errno);
    if (ret < 0) {
        return false;
    }

    //writeWord(0x00,0x8000);	// Reset
    uint8_t buffer[3];

	buffer[0] = 0x00;
	buffer[1] = (0x8000 & 0xFF00) >> 8;
	buffer[2] = 0x8000 & 0xFF;

	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data msgset[1];

	// Message Set 0: Write Command
	msgs[0].addr = deviceAddress;
	msgs[0].flags = 0;
	msgs[0].len = 3;
	msgs[0].buf = buffer;

	// Message Set contains 1 messages
	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 1;

	if (ioctl(busHandle, I2C_RDWR, &msgset) < 0) {
		printf("Write I2C failed\r\n");
		exit(1);
	}
    else {
        printf("Reset device OK\r\n");
    }

    //printf("Manufacturer ID:        0x%04X\r\n",readWord(0xFE));
    uint16_t buffer2;
    uint8_t command=0xFE;

	struct i2c_msg msgs2[2];
	struct i2c_rdwr_ioctl_data msgset2[1];

	// Message Set 0: Write Command
	msgs2[0].addr = devAddr;
	msgs2[0].flags = 0;
	msgs2[0].len = 1;
	msgs2[0].buf = &command;

	// Message Set 1: Read 2 bytes
	msgs2[1].addr = devAddr;
	msgs2[1].flags = I2C_M_RD | I2C_M_NOSTART;
	msgs2[1].len = 2;
	msgs2[1].buf = (unsigned char *)&buffer2;

	// Message Set contains 2 messages
	msgset2[0].msgs = msgs2;
	msgset2[0].nmsgs = 2;

	if (ioctl(busHandle, I2C_RDWR, &msgset2) < 0) {
		lastErrorString=strerror(errno);
        std::cerr << "readWord() error " << lastErrorString << std::endl;
        return 0xFF;
	}

	printf("Manufacturer ID:        0x%04X\r\n",bswap_16(buffer2));

//	printf("Die ID Register:        0x%04X\r\n",readWord(0xFF));
//	printf("Configuration Register: 0x%04X\r\n",readWord(0x00));
//	printf("\r\n");

    return true;
}

bool DI2C::writeWord(uint8_t command, uint16_t data)
{
	uint8_t buffer[3];

	buffer[0] = command;
	buffer[1] = (data & 0xFF00) >> 8;
	buffer[2] = data & 0xFF;

	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data msgset[1];

	// Message Set 0: Write Command
	msgs[0].addr = devAddr;
	msgs[0].flags = 0;
	msgs[0].len = 3;
	msgs[0].buf = buffer;

	// Message Set contains 1 messages
	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 1;

	if (ioctl(busHandle, I2C_RDWR, &msgset) < 0) {
		lastErrorString=strerror(errno);
        std::cerr << "writeWord() error " << lastErrorString << std::endl;
        return false;
	}

    return true;
}

uint16_t DI2C::readWord(uint8_t command)
{
	uint16_t buffer;

	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data msgset[1];

	// Message Set 0: Write Command
	msgs[0].addr = devAddr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &command;

	// Message Set 1: Read 2 bytes
	msgs[1].addr = devAddr;
	msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
	msgs[1].len = 2;
	msgs[1].buf = (unsigned char *)&buffer;

	// Message Set contains 2 messages
	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 2;

	if (ioctl(fd, I2C_RDWR, &msgset) < 0) {
		lastErrorString=strerror(errno);
        std::cerr << "readWord() error " << lastErrorString << std::endl;
        return 0xFF;
	}

	return(bswap_16(buffer));
}
*/
/*
uint32_t i2c_read_buf(uint32_t i2c_master_port, uint8_t address, uint8_t command, uint8_t *buffer, uint8_t len)
{
	//uint16_t buffer;

	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data msgset[1];

	// Message Set 0: Write Command
	msgs[0].addr = address;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &command;

	// Message Set 1: Read len bytes
	msgs[1].addr = address;
	msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
	msgs[1].len = len;
	msgs[1].buf = buffer;

	// Message Set contains 2 messages
	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 2;

	if (ioctl(i2c_master_port, I2C_RDWR, &msgset) < 0) {
		printf("Read I2C failed\r\n");
		exit(1);
	}

}
*/