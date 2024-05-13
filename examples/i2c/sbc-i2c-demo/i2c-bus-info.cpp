#include <iostream>
//#include <dservo>
//#include <dgpio.h>
#include <di2c>

int main(int argc, char** argv) {
    int busID=0;
    
    if (argc == 2) {
        busID=atoi(argv[1]);
    }
    else {
        std::cout << "Missing I2C bus ID" << std::endl;
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
