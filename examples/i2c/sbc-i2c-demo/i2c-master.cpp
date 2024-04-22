#include <iostream>
#include <dutils>
#include <di2cmaster>
#include <dmpacket>

int main(int argc, char** argv)
{
    std::vector<DI2CBus::DI2CAdaper> adapterList;
    DI2CBus::scanI2CBusses(adapterList);

    printf("Dev\t%-10s\t%-32s\t%s\n","Type","Name","Algo");
    printf("---------------------------------------------------------------------------\n");
    for (auto adapter : adapterList) {
		printf("i2c-%d\t%-10s\t%-32s\t%s\n",adapter.nr,adapter.funcs.c_str(),adapter.name.c_str(),adapter.algo.c_str());
	}

/*
    std::cout << "Start reading sensor on /dev/i2c-" << busID << " address " << devAddr << std::endl;
    std::cout << "Press CTRL+C to stop" << std::endl;

    DI2CMaster attiny(i2c.handle(),32);
    DMPacket rxPacket;
*/
/*    
    rxPacket.PushWord(0x0000);
    attiny.askForBuf(0x1,0x1,rxPacket.BufferRaw(),2);
    std::cout << "askForBuf() -> " << attiny.getLastError() << std::endl;
    std::cout << rxPacket.ReadInt16(0) << std::endl;

    rxPacket.Clear();
    uint16_t r=attiny.recvWord(1);
    std::cout << "recvWord() -> " << attiny.getLastError() << std::endl;
    rxPacket.PushWord(r);
    std::cout << rxPacket.ReadInt16(0) << std::endl;
*/
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
