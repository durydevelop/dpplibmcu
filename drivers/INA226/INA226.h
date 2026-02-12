/*
 * INA226 - TI Current/Voltage/Power Monitor Code
 * Copyright (C) 2021 Craig Peacock
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 * 
 * Modified in 2024 by Fabio Durigon.
 * 
big-endian (MSB first) 

Configuration register:
BIT NO.   D15 D14 D13 D12 D11  D10   D9    D8       D7     D6      D5     D4     D3    D2    D1    D0
BIT NAME  RST  —   —   —  AVG2 AVG1 AVG0 VBUSCT2 VBUSCT1 VBUSCT0 VSHCT2 VSHCT1 VSHCT0 MODE3 MODE2 MODE1
POR VALUE  0   1   0   0   0    0    0      1       0       0       1      0      0     1     1     1
POR HEX    0x4127

D11 D10  D9 NUMBER OF AVERAGES
 0   0   0    1
 0   0   1    4
 0   1   0    16
 0   1   1    64
 1   0   0    128
 1   0   1    256
 1   1   0    512
 1   1   1    1024

7.1.6 Calibration Register (05h) (Read/Write)
This register provides the device with the value of the shunt resistor that is present to create the measured
differential voltage. The register also sets the resolution of the Current Register. Programming this register
sets the Current_LSB and the Power_LSB. This register is also used in overall system calibration. See the
Programming the Calibration Register for additional information on programming the Calibration Register.
Table 7-11. Calibration Register (05h) (Read/Write) Description
BIT NO.    D15  D14   D13   D12 D11 D10 D9 D8 D7 D6 D5 D4 D3 D2 D1 D0
BIT NAME   —    FS14  FS13  FS12 FS11 FS10 FS9 FS8 FS7 FS6 FS5 FS4 FS3 FS2 FS1 FS0
POR VALUE  0     0      0    0 0 0 0 0 0 0 0 0 0 0 0 0


 */

#ifndef INA226_H
#define INA226_H

// Register addresses
#define INA226_REG_CFG		        0x00
#define INA226_REG_SHUNT_VOLT	    0x01
#define INA226_REG_BUS_VOLT	        0x02
#define INA226_REG_POWER	        0x03
#define INA226_REG_CURRENT	        0x04
#define INA226_REG_CAL		        0x05
#define INA226_REG_MASKEN	        0x06
#define INA226_REG_ALERT_LMT	    0x07
#define INA226_REG_MANUFACTURER_ID	0xFE
#define INA226_REG_DIE_ID		    0xFF

//  Returned by setMaxCurrent
#define INA226_ERR_NONE                   0x0000
#define INA226_ERR_SHUNTVOLTAGE_HIGH      0x8000
#define INA226_ERR_MAXCURRENT_LOW         0x8001
#define INA226_ERR_SHUNT_LOW              0x8002
#define INA226_ERR_NORMALIZE_FAILED       0x8003
#define INA226_ERR_NOT_READY              0x8004

#define INA226_MINIMAL_SHUNT_OHM          0.001

#include <di2cmaster>
#include <map>

class INA226 : public DI2CMaster {
    public:
        enum Avaraging {
            AVG_1 = 0b000,
            AVG_4 = 0b001,
            AVG_16 = 0b010,
            AVG_64 = 0b011,
            AVG_128 = 0b100,
            AVG_256 = 0b101,
            AVG_512 = 0b110,
            AVG_1024 = 0b111
        };

        INA226(uint8_t deviceAddr, float shuntOhm, DI2CBusHandle i2cBusHandle);
        ~INA226();
        bool begin(float maxCurrent);
        bool isReady(void);
        bool reset(void);
        bool setMaxCurrent(float maxCurrent, bool normalize);
        bool setAvaraging(INA226::Avaraging avgMask);
        float getBusVoltage(void);
        float getShuntVoltage(void);
        float getCurrent(void);
        float getPower(void);

        uint32_t getManufacturerID(void);
        uint32_t getDieID(void); 
        std::string getInfo(void);

    private:
        bool readConfig(void);

        std::map<uint16_t, std::string> INA226ErrorMap = {
            {INA226_ERR_NONE, "No error"},
            {INA226_ERR_SHUNTVOLTAGE_HIGH, "Shunt voltage too high"},
            {INA226_ERR_MAXCURRENT_LOW, "Max current too low"},
            {INA226_ERR_SHUNT_LOW, "Shunt resistance too low"},
            {INA226_ERR_NORMALIZE_FAILED, "Normalization failed"},
            {INA226_ERR_NOT_READY, "INA226 not ready"}
        };

        union REG_CFG {
            uint16_t value;
            struct {
                uint16_t mode   : 3;  // D2-D0   (bit 0-2)   Operating mode
                uint16_t vshct  : 3;  // D5-D3   (bit 3-5)   Shunt voltage conversion time
                uint16_t vbusct : 3;  // D8-D6   (bit 6-8)   Bus voltage conversion time
                uint16_t avg    : 3;  // D11-D9  (bit 9-11)  Averaging bits
                uint16_t reserved : 3;// D14-D12 (bit 12-14) Reserved bits
                uint16_t rst    : 1;  // D15     (bit 15)    Reset
            } reg;
        } cfg;
        
        uint8_t devAddr;
        float shuntR;   // Shunt R value      -> Ohm
        float lsbI = 0; // Current resolution -> A/bit
        float maxI = 0; // Max current        -> A
};

#endif
