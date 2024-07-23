#include <iostream>
#include <sstream>
#include <dutils>
#include <di2c>
#include <dmpacket>

int busID=-1;
uint8_t startAddr=0x08;
uint8_t endAddr=0x77;
DI2CBus::DScanMode mode=DI2CBus::SCAN_MODE_READ;

// *********************************************** Command line *************************************************************************
void showUsage(std::string binaryName)
{
    std::cout <<
        "Usage: " << binaryName << " <bus id> [options]" << std::endl <<
        "    -h, --help                 Show this help" << std::endl <<
        "    <bus id>                   Id of the I2C bus. Usually is 1 but you can retrive availables busses using 'i2c-bus-info' tools or by command 'ls /dev/i2c-*'" << std::endl <<
        "    -s, --start-addr           Scanning start address (default 0x08)" << std::endl <<
        "    -e, --end-addr             Scanning end address (default 0x77)" << std::endl <<
        "    -m, --mode                 Scan mode: \"r\" for read mode or \"w\" for write mode (default \"r\")" << std::endl <<
            "Example:" << std::endl <<
            "Scan /dev/i2c-1 bus from 0x08 to 0x77 using read mode:" << std::endl <<
            binaryName << " 1" << std::endl <<
            "Scan /dev/i2c-1 bus from 0x00 to 0x7F using write mode:" << std::endl <<
            binaryName << " 1 -s0x00 -e0x77 -m w" << std::endl;
}

bool parseCmdLine(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Missing parameters" << std::endl;
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
        if (sArg[0] == '-' && itArg+1 == std::end(args)) {
            std::cerr << "Option " << sArg << " needs a value"<< std::endl;
            return false;
        }
        else if (sArg == "-s" || sArg == "--start-addr") {
            // Custom port
            std::istringstream(*(++itArg)) >> std::hex >> startAddr;
        }
        else if (sArg == "-e" || sArg == "--end-addr") {
            // Costom bind address
            std::istringstream(*(++itArg)) >> std::hex >> endAddr;
        }
        else if (sArg == "-m" || sArg == "--mode") {
            // Custom web socket url
            std::string m=*(++itArg);
            if (m == "r" || m == "R") {
                mode=DI2CBus::SCAN_MODE_READ;
            }
            else if (m == "w" || m == "W") {
                mode=DI2CBus::SCAN_MODE_WRITE;
            }
            else {
                std::cerr << "Option " << sArg << " is not supported"<< std::endl;
            }
            return true;
        }
        else {
            if (busID == -1) {
                busID=atoi(sArg.c_str());
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

int main(int argc, char* argv[]) {
    if (!parseCmdLine(argc,argv)) {
        exit(1);
    }

    DI2CBus i2c(busID);
    std::cout << "Capability of bus /dev/i2c-" << busID << ":" << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << i2c.getInfo() << std::endl;
    std::cout << "-----------------------------" << std::endl;
    printf("Scanning from 0x%02X to 0x%02X using %s mode: \r\n",startAddr,endAddr,std::string(mode == DI2CBus::SCAN_MODE_READ ? "READ" : "WRITE").c_str());
    int count=i2c.scanForDevices();
    if (count == -1) {
        std::cerr << "Scan error: " << i2c.getLastError() << std::endl;
    }
    else if (count == 0) {
        std::cout << "No device found";
    }
    else {
        if (count == 1) {
            std::cout << "Found 1 device at ";
        }
        else {
            std::cout << "Found " << count << " devices:" << std::endl;
        }

        for ( uint8_t id : i2c.devices) {
            printf("0x%02X ",id);
        }

        std::cout << std::endl;
    }

    return 0;
}
