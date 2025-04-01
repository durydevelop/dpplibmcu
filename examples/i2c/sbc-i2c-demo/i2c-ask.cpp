#include <iostream>
#include <dutils>
#include <di2cmaster>
#include <dmpacket>
#include <sstream>

int busID=-1;
int slaveAddr=-1;
int cmdReg=-1;
int numBytes=4;

// *********************************************** Command line *************************************************************************
void showUsage(std::string binaryName)
{
    std::cout <<
        "Send an i2c command and read for reply (usually called 'read device registry')." << std::endl <<
        "Usage: " << binaryName << " <bus id> <slave addr> [options]" << std::endl <<
        "    <bus id>       Id of the I2C bus. Usually is 1 but you can retrive availables busses using 'i2c-bus-info' tools or by command 'ls /dev/i2c-*'" << std::endl <<
        "    <slave addr>   Address of the device to send command (e.g. 0x10)"
        "    <commad>       Command byte: a single byte to send to slave device (aka register)"
        "    -h, --help     Show this help" << std::endl <<
        "    -bN,           Number of bytes of the responce to show. Default is 4 (DWORD) if the device return less bytes than request, FF will be added." << std::endl <<
        "Example:" << std::endl <<
        "Send 0x04 command to device ID 0x10 and print first 2 bytes of responce (that usually is explained as: read 2 bytes from 0x04 register of the 0x10 device)" << std::endl <<
        binaryName << " 1 0x10 0x0A -b2" << std::endl;
}

bool parseCmdLine(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Error: missing parameters" << std::endl << std::endl;
        showUsage(argv[0]);
        return false;
    }
    const std::vector<std::string> args(argv + 1, argv + argc);

    for(auto itArg = std::begin(args); itArg != std::end(args); itArg++) {
        std::string sArg=*itArg;
        if (sArg == "-h" || sArg == "--help") {
            showUsage(argv[0]);
            return false;
        }

        // Check for option type
        /*if (sArg[0] == '-' && itArg+1 == std::end(args)) {
            std::cerr << "Option " << sArg << " needs a value"<< std::endl;
            return false;
        }
        else */
        if (sArg[0] == '-' && sArg.size() >= 1) {
            // Start of option
            if (sArg[1] == 'b') {
                // Number of bytes to print
                if (sArg.size() > 2) {
                    // Value without space
                    std::istringstream(sArg.substr(2)) >> numBytes;
                    std::cout << "numBytes=" << numBytes << std::endl;
                }
                else if (itArg+1 != std::end(args)) {
                    // Value after space (as arg)
                    std::istringstream(*(++itArg)) >> numBytes;
                    std::cout << "numBytes=" << numBytes << std::endl;
                }
                else {
                    std::cerr << "Option " << sArg << " needs a value"<< std::endl;
                    return false;
                }
            }
        }
        else {
            if (busID == -1) {
                // First arg
                busID=atoi(sArg.c_str());
            }
            else if (slaveAddr == -1) {
                // Second arg
                std::istringstream(sArg) >> std::hex >> slaveAddr;
            }
            else if (cmdReg == -1) {
                // First arg
                std::istringstream(sArg) >> std::hex >> cmdReg;
            }
            else {
                std::cerr << "option " << sArg << " is not valid" << std::endl;
                return false;
            }
        }
    }
    return true;
}
// **************************************************************************************************************************************

int main(int argc, char** argv)
{
    if (!parseCmdLine(argc,argv)) {
        exit(1);
    }

    DI2CBus i2c(busID);
    std::cout << "i2c bus init:        " << i2c.getLastError() << std::endl;
    DI2CMaster device(i2c.handle());
    std::cout << "i2c device init:     " << device.getLastError() << std::endl;
    DMPacket rxPacket;
    rxPacket.clear();
    uint8_t b[numBytes];
    device.askForBuf(slaveAddr,cmdReg,b,numBytes);
    std::cout << "i2c device sent cmd: " << device.getLastError() << std::endl;
    rxPacket.setBuffer(b,numBytes);
    std::cout << "Received:            " << rxPacket.toHexString() << std::endl;
    return 0;
}
