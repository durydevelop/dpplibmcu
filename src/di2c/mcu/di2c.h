#ifndef DI2C_H
#define DI2C_H

#include <Arduino.h>

class DI2CMaster
{
    public:
        DI2CMaster(uint8_t sda_pin, uint8_t scl_pin);
        void begin();
        bool startWrite(uint8_t slaveAddr);
        bool startRead(uint8_t slaveAddr);
        bool write(uint8_t data);
        bool read(uint8_t &data, bool ack);
        void stop();

        // Arduino Wire wrapper
        bool beginTransmission(uint8_t slaveAddr) { return startWrite(slaveAddr); }
        bool endTransmission() { stop(); return true; }

        void i2cStart();
        void i2cStop();
        bool i2cSend(uint8_t byte);
        uint8_t i2cRecv(bool send_ack);

        bool sendByte(uint8_t slaveAddr, uint8_t data);
        bool sendWord(uint8_t slaveAddr, uint16_t data);
        bool sendDword(uint8_t slaveAddr, uint32_t data);
        bool sendBuf(uint8_t slaveAddr, const uint8_t *data, size_t len);
        
    private:
        uint8_t sda;
        uint8_t scl;
       
        void sdaHigh();
        void sdaLow();
        void sclHigh();
        void sclLow();
};

#endif