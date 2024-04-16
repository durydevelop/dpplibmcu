#ifndef DI2C_H
#define DI2C_H

#include <cstdint>
#include <string>

typedef int DI2CBusHandle;

class DI2C {
    public:
        DI2C(uint8_t busID);
        ~DI2C();

        DI2CBusHandle handle(void);
        bool isReady(void);
        std::string getLastError(void);
        std::string getInfo(void);

        //bool begin(uint8_t deviceAddress);

        //uint8_t i2c_read_byte(uint32_t i2c_master_port, uint8_t address, uint8_t command);
        //uint32_t i2c_write_byte(uint32_t i2c_master_port, uint8_t address, uint8_t command, uint8_t data);

        //uint16_t readWord(uint8_t command);
        //bool writeWord(uint8_t command, uint16_t data);

        

        //uint32_t i2c_read_buf(uint32_t i2c_master_port, uint8_t address, uint8_t command, uint8_t *buffer, uint8_t len);
        //uint32_t i2c_write_buf(uint32_t i2c_master_port, uint8_t address, uint8_t command, uint8_t *data, uint8_t len);

    private:
        std::string busName;
        uint8_t devAddr;
        DI2CBusHandle busHandle;
        int lastResult;

        
};

#endif