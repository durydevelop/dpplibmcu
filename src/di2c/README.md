### I2C support for linux SBC computer
This module add full support for I2c to linux based SBC (raspberry, radxa, etc...)
It can also be used as stan-alone library, you only need to to copy the di2c folder to your project and add it,
for example with cmake: add_subdirectory(di2c).

## Futures
* [x] Scanner for available buses
* [x] Scanner for available devices on bus
* [x] Act as master for comunication
* [ ] Act as slave.

## Usage:
Scan available I2C buses:
```cpp
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
```

Show bus capibilities info:
```cpp
    // Simple istantiate a DI2CBus class on busID
    DI2CBus i2c(busID);
    // Show info
    std::cout << i2c.getInfo() << std::endl;
```
For more, look into I2C [examples](examples/i2c/sbc-i2c-demo/) folder.