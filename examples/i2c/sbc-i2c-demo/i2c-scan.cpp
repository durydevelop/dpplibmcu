#include <iostream>
#include <dutils>
#include <di2c>
#include <dmpacket>

void showUsage(std::string binaryName)
{
    std::cout << "Usage: " << binaryName << " <i2c bus> [start address = 0x08] [end address = 0x77] [mode]" << std::endl <<
            "    <i2c bus> is the id of i2c device handled by /dev/i2c-..." << std::endl <<
            "    [star address] scanning start address (default 0x08)" << std::endl <<
            "    [end address] scanning end address (default 0x77)" << std::endl <<
            "    [mode] scan mode: \"r\" for read mode or \"w\" for write mode (default \"r\")" << std::endl <<
            "Example:" << std::endl <<
            "Scan from 0x08 to 0x77 (119) using read mode on /dev/i2c-1 bus:" << std::endl <<
            binaryName << " 1" << std::endl <<
            "Scan from 0x00 to 0x7F (127) using write mode on /dev/i2c-1 bus:" << std::endl <<
            binaryName << " 1 0 127 w" << std::endl;
}

int main(int argc, char** argv) {

    int busID=1;
    uint8_t startAddr=0x08;
    uint8_t endAddr=0x77;
    DI2CBus::DScanMode mode=DI2CBus::SCAN_MODE_READ;

    if (argc > 1) {
        for (int ixA=1; ixA<argc; ixA++) {
            std::string arg(argv[ixA]);
            if (arg.substr(0,2) == "-h" || arg.substr(0,6) == "--help") {
                // Show usage
                showUsage(argv[0]);
                exit(1);
            }
            else if (ixA == 1) {
                busID=atoi(argv[ixA]);
            }
            else if (ixA == 2) {
                startAddr=atoi(argv[ixA]);
            }
            else if (ixA == 3) {
                endAddr=atoi(argv[ixA]);
            }
            else if (ixA == 4) {
                if (std::string(argv[ixA]) == "r") {
                    mode=DI2CBus::SCAN_MODE_READ;
                }
                if (std::string(argv[ixA]) == "w") {
                    mode=DI2CBus::SCAN_MODE_WRITE;
                }
                else {
                    showUsage(argv[0]);
                    exit(1);
                }
            }
        }
    }
    else {
        showUsage(argv[0]);
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
