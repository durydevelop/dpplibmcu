#include <iostream>
#include <dutils>
#include <di2cmaster>
#include <dmpacket>

int main(int argc, char** argv)
{

    int busID=0;
    
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
    DMPacket rxPacket;

/*    
    rxPacket.PushWord(0x0000);
    attiny.askForBuf(0x1,0x1,rxPacket.BufferRaw(),2);
    std::cout << "askForBuf() -> " << attiny.getLastError() << std::endl;
    std::cout << rxPacket.ReadInt16(0) << std::endl;
*/
    rxPacket.Clear();
    uint16_t r=attiny.askForWord(0x10,0);
    std::cout << "askForWord() -> " << attiny.getLastError() << std::endl;
    rxPacket.PushWord(r);
    std::cout << "data: " << rxPacket.ReadInt16(0) << std::endl;

/*
    std::string buf="abcd12345678901234567890123411dcba";
    do {
        //attiny.sendByte(8,'a');
        attiny.sendBuf(8,(uint8_t *)buf.data(),buf.size());
        std::cout << "sendBuf() -> " << attiny.getLastError() << std::endl;
        delay(1000);
    }while(true);
*/
    return 0;
}
