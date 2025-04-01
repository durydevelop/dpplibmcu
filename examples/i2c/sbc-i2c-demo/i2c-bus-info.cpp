#include <iostream>
#include <di2c>

void showUsage(std::string ProgramName) {
    std::cout <<
        "Print i2c bus capibilities." << std::endl <<
        "Usage: "+ProgramName+" <Bus ID> " << std::endl <<
        "<Bus ID> is id of the I2CBus. Usually is 1 but you can retrive availables busses using 'i2c-bus-info' tools or by command 'ls /dev/i2c-*'" << std::endl;
}

int main(int argc, char** argv) {
    int busID=0;
    
    if (argc == 2) {
        busID=atoi(argv[1]);
    }
    else {
        std::cout << "Missing I2C bus ID" << std::endl;
        showUsage(argv[0]);
        exit(1);
    }

    std::cout << "Capibilities of bus /dev/i2c-" << busID << std::endl;
    std::cout << "------------------------------" << std::endl;
    // Simple istantiate a DI2CBus class on busID
    DI2CBus i2c(busID);
    // Show info
    std::cout << i2c.getInfo() << std::endl;
    return 0;
}
