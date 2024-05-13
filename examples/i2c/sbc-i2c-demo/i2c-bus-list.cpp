#include <iostream>
#include <dutils>
#include <di2cmaster>
#include <dmpacket>

int main(int argc, char** argv)
{
    // Vector of adapters for result
    std::vector<DI2CBus::DI2CAdaper> adapterList;
    // Scan system
    DI2CBus::scanI2CBusses(adapterList);

    // Show result
    printf("Dev\t%-10s\t%-32s\t%s\n","Type","Name","Algo");
    printf("---------------------------------------------------------------------------\n");
    for (auto adapter : adapterList) {
	    printf("i2c-%d\t%-10s\t%-32s\t%s\n",adapter.nr,adapter.funcs.c_str(),adapter.name.c_str(),adapter.algo.c_str());
    }

    return 0;
}
