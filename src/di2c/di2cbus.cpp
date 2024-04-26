#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <map>
#include <iostream>
#include "di2cbus.h"
#include <dutils>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

#define DI2C_BUS_DEV_PREFIX "/dev/i2c"
enum DI2CAdapterType                        { ADAPTER_DUMMY, ADAPTER_ISA, ADAPTER_I2C,   ADAPTER_SMBUS,   ADAPTER_UNKNOWN };
const std::vector<std::string> DI2CAdapterFuncs=  { "dummy",       "isa",       "i2c",         "smbus",         "unknown"       };
const std::vector<std::string> DI2CAdapterAlgo=   { "Dummy Bus",   "ISA bus",   "I2C adapter", "SMBus adapter", "N/A"           };
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

DI2CBus::DI2CBus(uint8_t busID)
{
    //busHandle=DI2CBus::openI2CBus(busID);
    busName=DI2C_BUS_DEV_PREFIX "-" + std::to_string(busID);
        busHandle = open(busName.c_str(), O_RDWR);
        std::cout << strerror(errno) << std::endl;
    lastErrorString=strerror(errno);
}

DI2CBus::~DI2CBus()
{
    if (close(busHandle) < 0) {
        std::cerr << "Closing " << busName << " error: " << strerror(errno) << std::endl;
    }
}

int DI2CBus::scanForDevices(DScanMode scanMode, uint8_t startAddr, uint8_t endAddr)
{
    for (uint8_t slaveAddr=startAddr; slaveAddr<=endAddr; slaveAddr++) {
        i2c_msg message;
        if (scanMode == SCAN_MODE_READ) {
            // Recv byte
            uint8_t recvByte=0xFF;
            message={ slaveAddr, I2C_M_RD, 1, &recvByte };
        }
        else if (scanMode == SCAN_MODE_WRITE) {
            uint8_t data='+';
            message={slaveAddr,0,1,&data};
        }
        else {
            lastErrorString="Scan mode unknown";
            return -1;
        }
        struct i2c_rdwr_ioctl_data ioctlData={ &message, 1 };

        // Perform I/O
        int ret=ioctl(busHandle, I2C_RDWR, &ioctlData);
        lastErrorString=strerror(errno);
        if (ret >= 0) {
            devices.push_back(slaveAddr);
        }

        if (message.len != 1) {
            std::cout << "WARNING: msgs len missing real size" << std::endl;
        }
    }

    return devices.size();
}

DI2CBusHandle DI2CBus::handle(void)
{
    return busHandle;
}

bool DI2CBus::isReady(void)
{
    return busHandle >= 0;
}

std::string DI2CBus::getLastError(void)
{
    return lastErrorString;
}

std::string DI2CBus::getInfo(void)
{
    ulong funcs;
    int ret=ioctl(busHandle, I2C_FUNCS, &funcs);
    lastErrorString=strerror(errno);

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

int DI2CBus::openI2CBus(int busID)
{
    // Try first "/dev/i2c/%d"
    std::string busName=DI2C_BUS_DEV_PREFIX "/" + std::to_string(busID);
    int busHandle = open(busName.c_str(), O_RDWR);
    std::cout << strerror(errno) << std::endl;
    if (busHandle < 0 && (errno == ENOENT || errno == ENOTDIR)) {
        // Then "/dev/i2c-%d"
        busName=DI2C_BUS_DEV_PREFIX "-" + std::to_string(busID);
        busHandle = open(busName.c_str(), O_RDWR);
        std::cout << strerror(errno) << std::endl;
    }

    return busHandle;
}

int DI2CBus::scanI2CBusses(std::vector<DI2CBus::DI2CAdaper> &resultList)
{
    // Devices path where to search
    std::vector<std::string> FS_DEV_FILES = {
        "/proc/bus/i2c",
        "/proc/mounts"
    };
    
    int ixFound=-1;
    for (int ixPath=0; ixPath<FS_DEV_FILES.size(); ixPath++) {
        std::error_code ec;
        fs::file_status Status=fs::status(FS_DEV_FILES[ixPath],ec);
        if (fs::exists(Status)) {
            ixFound=ixPath;
            break;
        }
    }
    if (ixFound < 0) {
        std::cerr << "Linux version is not compatible" << std::endl;
        return -1;
    }

    if (ixFound == 0) {
        // "/proc/bus/i2c"
        //TODO
    }
    else if (ixFound == 1) {
        // open file "/proc/mounts"
        std::ifstream mountsFile;
        mountsFile.open(FS_DEV_FILES[ixFound]);
        if (!mountsFile.is_open()) {
            std::cerr << FS_DEV_FILES[1] << "open error: " << strerror(errno) << std::endl;
            return -1;
        }

        // Read content line by line
        std::string line;
        std::string sysfs;
        while(std::getline(mountsFile,line)) {
            std::stringstream ss(line);
            int ixItem=0;
            std::string lastValue;
            // Find for something like
            // sysfs /sys sysfs rw,nosuid,nodev,noexec,relatime 0 0
            //         ^    ^
            for (std::string value; std::getline(ss, value, ' '); ixItem++) {
                if (value == "sysfs" && lastValue == "/sys") {
                    sysfs=lastValue;
                    break;
                }
                if (ixItem == 2) {
                    break;
                }
                lastValue=value;
            }
        }

        mountsFile.close();

        if (sysfs.empty()) {
            std::cerr << "Linux version is not compatible: device not found in /proc/mounts" << std::endl;
            return -2;
        }

        // Read adapters list
        sysfs.append("/class/i2c-dev");
        for (fs::directory_iterator item(sysfs); item != fs::directory_iterator(); ++item) {
			if (fs::is_directory(item->status())) {
                // find sysfs/i2c-N/name
                // this should work for kernels 2.6.5 or higher and is preferred because is unambiguous
                fs::path adapterDir=item->path();
                fs::path nameFile=item->path() / "name";
                if (!fs::exists(fs::status(nameFile))) {
                    // non-ISA is much harder and this won't find the correct bus name if a driver has more than one bus
                    adapterDir=item->path() / "device" / "name";
                    for (fs::directory_iterator nameItem(adapterDir); nameItem != fs::directory_iterator(); ++nameItem) {
			            if (fs::is_directory(nameItem->status())) {
                            if (nameItem->path().stem().string().find("i2c-") == 0) {
                                nameFile=item->path() / "name";
                                if (!fs::exists(fs::status(nameFile))) {
                                    std::cerr << "Linux version is not compatible" << std::endl;
                                    return -3;
                                }
                            }
                        }
                    }
                }
                // Adapter found
                DI2CBus::DI2CAdaper adapter;
                // Cut "i2c-" from dir name and get nr
                adapter.nr=atoi(adapterDir.stem().string().substr(4).c_str());
                // read name from file
                std::ifstream nameInputFile(nameFile);
                std::getline(nameInputFile,adapter.name);
                nameInputFile.close();
                // Type
                DI2CAdapterType adapterType;
                if (adapter.name.substr(0,4) == "ISA ") {
                    adapterType=ADAPTER_ISA;
                }
                else {
                    // Get adapter funcs
                    int busHandle=DI2CBus::openI2CBus(adapter.nr);
                    if (busHandle < 0) {
                        std::cerr << "Cannot open " << adapter.name << " : " << strerror(errno) << std::endl;
                        return -4;
                    }
                    ulong funcs;
                    int ret=ioctl(busHandle, I2C_FUNCS, &funcs);
                    if (ret < 0) {
                        adapterType=ADAPTER_UNKNOWN;
                    }
                    else if (funcs & I2C_FUNC_I2C) {
                        adapterType=ADAPTER_I2C;
                    }
                    else if (funcs & (I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA)) {
                        adapterType=ADAPTER_SMBUS;
                    }
                    else {
                        adapterType=ADAPTER_DUMMY;
                    }
                    close(busHandle);
                }
                adapter.funcs=DI2CAdapterFuncs[adapterType];
                adapter.algo=DI2CAdapterAlgo[adapterType];
                resultList.emplace_back(adapter);
            }
        }
    }

    return resultList.size();
}
