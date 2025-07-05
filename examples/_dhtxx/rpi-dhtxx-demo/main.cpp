#include <iostream>
//#include <dservo>
//#include <dgpio.h>
#include <dhtxx>

int main(int argc, char** argv) {

    int pwmPin=0;
    if (argc == 2) {
        pwmPin=atoi(argv[1]);
    }
    else {
        std::cout << "Missing argument." << std::endl;
        exit(1);
    }

    if (pwmPin <= 0 || pwmPin > 32) {
        std::cout << "Pin nr not valid." << std::endl;
        exit(1);
    }

   DGpioChip chip(0);
    if (!chip.isReady()) {
        std::cout << "DGpioChip not ready: " << chip.getLastError() << std::endl;
    }
    else {
        std::cout << chip.getInfo() << std::endl;
    }

    DHTXX dht(pwmPin,chip.handle());
    dht.begin(DHTXX::DHT_11);
    std::cout << dht.getLastError() << std::endl;
   
    std::cout << "Start DHT11 on pin " << pwmPin << std::endl;
    std::cout << "Press CTRL+C to stop" << std::endl;
    
    do{
        if (dht.read() == 0) {
            std::cout << "h=" << dht.humidity << "t=" << dht.temp << std::endl;
        }
        delay(200);
    }while(true);
    
    return 0;
}
