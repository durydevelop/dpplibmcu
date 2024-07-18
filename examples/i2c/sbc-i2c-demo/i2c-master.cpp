#include <iostream>
#include <dutils>
#include <di2cmaster>
#include <dmpacket>

int main(int argc, char** argv)
{

    int busID=1;
/*    
    if (argc == 2) {
        busID=atoi(argv[1]);
    }
    else {
        std::cout << "Missing I2C bus ID" << std::endl;
        exit(1);
    }

    std::cout << "Capility of bus /dev/i2c-" << busID << std::endl;
    std::cout << "------------------------------" << std::endl;
    DI2CBus i2c(busID);
    std::cout << i2c.getInfo() << std::endl;
    std::cout << i2c.getLastError() << std::endl;
    DI2CMaster attiny(i2c.handle(),32);
    
*/
/*    
    rxPacket.PushWord(0x0000);
    attiny.askForBuf(0x1,0x1,rxPacket.BufferRaw(),2);
    std::cout << "askForBuf() -> " << attiny.getLastError() << std::endl;
    std::cout << rxPacket.ReadInt16(0) << std::endl;
*/
    DI2CBus i2c(busID);
    DI2CMaster attiny(i2c.handle(),32);
    DMPacket rxPacket;
    rxPacket.clear();
    uint8_t b[4];
    attiny.askForBuf(0x10,1,b,4);
    rxPacket.setBuffer(b,4);
    std::cout << "buff=" << rxPacket.toHexString() << " " << attiny.getLastError() << std::endl;
    printf("Temp     = %.02f \n", rxPacket.readFloat(0));

/*
    std::string buf="abcd12345678901234567890123411dcba";
    do {
        printf("Temp     = %.02d ", attiny.askForWord(0x10,1));
        //printf("Humidity = %.02f\n", attiny.askForWord(0x10,2));
        //attiny.sendBuf(0x10,(uint8_t *)buf.data(),buf.size());
        std::cout << "sendBuf() -> " << attiny.getLastError() << std::endl;
        delay(1000);
    }while(true);
*/
    return 0;
}
