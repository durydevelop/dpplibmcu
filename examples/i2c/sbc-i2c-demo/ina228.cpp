#include <iostream>
#include <sstream>
#include <dutils>
#include <INA228/INA228.h>

int main(int argc, char** argv) {

    int busID=0;
    int devAddr=0x40;
    
    if (argc == 3) {
        busID=atoi(argv[1]);
        std::istringstream(argv[2]) >> std::hex >> devAddr;
    }
    else {
        std::cout <<
            "Usage: " << argv[0] << " <i2c bus> <INA226 address>" << std::endl <<
            "    <i2c bus> is the id of i2c device handled by /dev/i2c-..." << std::endl <<
            "    <INA226 address> is the i2c address of the sensor, default 0x40" << std::endl <<
            "Example:" << std::endl <<
            "Starts reading on /dev/i2c-1 at address 0x40" << std::endl <<
            argv[0] << " 1 0x40" << std::endl;
            
        exit(1);
    }

    std::cout << "Capibility of bus /dev/i2c-" << busID << ":" << std::endl;
    DI2CBus i2c(busID);

    INA228 ina228(devAddr,i2c.handle());
    ina228.begin();
    std::cout << "INA228 sensor info:" << std::endl << ina228.getInfo() << std::endl;

    std::cout << "Start reading sensor on /dev/i2c-" << busID << " address " << devAddr << std::endl;
    std::cout << "Press CTRL+C to stop" << std::endl;

    do{
        printf("Voltage = %.02f V ", ina228.voltage());
        printf("Current = %.02f A ", ina228.current());
        printf("Power   = %.02f W\r\n", ina228.power());
        printf("Die Temp = %.02f V ", ina228.dietemp());
        printf("Shunt Voltage = %.02f A ", ina228.shuntvoltage());
        printf("Energy   = %.02f W", ina228.energy());
        printf("Charge   = %.02f W\r\n", ina228.charge());
        delay(1000);
    }while(true);

    return 0;
}
