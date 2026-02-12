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
 * Modified in 2024-2025 by Fabio Durigon.
 * 
 * https://github.com/RobTillaart/INA226.git
 */

#include "INA226.h"
#include <cmath>
//#define printDebug

INA226::INA226(uint8_t deviceAddr, float shuntOhm, DI2CBusHandle i2cBusHandle) : DI2CMaster(i2cBusHandle)
{
    devAddr = deviceAddr;
    shuntR = shuntOhm;
}

INA226::~INA226()
{
}

bool INA226::begin(float maxCurrent)
{
    if (!readConfig()) {
        return false;
    }
      
    bool ret=setMaxCurrent(maxCurrent,true);
    if (!ret) {
        ret=setMaxCurrent(maxCurrent,false);
    }

    return ret;
}

bool INA226::readConfig(void)
{
    cfg.value = askForWord(devAddr, INA226_REG_CFG);
    if (cfg.value == 0xFFFF) {
        return false;
    }
    return true;
}

bool INA226::isReady(void)
{
    uint16_t id = askForWord(devAddr, INA226_REG_MANUFACTURER_ID);
    return id != 0xFFFF;
}

bool INA226::reset(void)
{
    readConfig();
    cfg.reg.rst = 1;
    return writeWord(devAddr, INA226_REG_CFG, cfg.value);
}

/**
 * @brief 
 * from https://github.com/RobTillaart/INA226.git
 * @param maxAmpere 
 * @param normalize 
 * @return int 
 */
bool INA226::setMaxCurrent(float maxAmpere, bool normalize)
{
    float shuntV = maxAmpere * shuntR;
    if (shuntV > 0.08190) {
        lastErrorString=INA226ErrorMap[INA226_ERR_SHUNTVOLTAGE_HIGH];
        return false;
    }
    if (maxAmpere < 0.001) {
        lastErrorString=INA226ErrorMap[INA226_ERR_MAXCURRENT_LOW];
        return false;
    }
    if (shuntR < INA226_MINIMAL_SHUNT_OHM) {
        lastErrorString=INA226ErrorMap[INA226_ERR_SHUNT_LOW];
        return false;
    }

    // Current resolution
    lsbI = maxAmpere * 3.0517578125e-5; //  maxAmpere / 32768;

#ifdef printdebug
    Serial.println();
    Serial.print("normalize:\t");
    Serial.println(normalize ? " true" : " false");
    Serial.print("initial current_LSB:\t");
    Serial.print(lsbI * 1e+6, 1);
    Serial.println(" uA / bit");
#endif

    //  normalize the LSB to a round number
    //  LSB will increase
    if (normalize)
    {
        /*
           check if maxAmpere (normal) or shunt resistor
           (due to unusual low resistor values in relation to maxAmpere) determines currentLSB
           we have to take the upper value for currentLSB

           calculation of currentLSB based on shunt resistor and calibration register limits (2 bytes)
           cal = 0.00512 / ( shunt * currentLSB )
           cal(max) = 2^15-1
           currentLSB(min) = 0.00512 / ( shunt * cal(max) )
           currentLSB(min) ~= 0.00512 / ( shunt * 2^15 )
           currentLSB(min) ~= 2^9 * 1e-5 / ( shunt * 2^15 )
           currentLSB(min) ~= 1e-5 / 2^6 / shunt
           currentLSB(min) ~= 1.5625e-7 / shunt
        */
        if (1.5625e-7 / shuntR > lsbI)
        {
            //  shunt resistor determines current_LSB
            //  => take this a starting point for current_LSB
            lsbI = 1.5625e-7 / shuntR;
        }

#ifdef printdebug
        Serial.print("Pre-scale current_LSB:\t");
        Serial.print(lsbI * 1e+6, 1);
        Serial.println(" uA / bit");
#endif

        //  normalize lsbI to a value of 1, 2 or 5 * 1e-6 to 1e-3
        //  convert float to int
        uint16_t currentLSB_uA = float(lsbI * 1e+6);
        currentLSB_uA++; //  ceil() would be more precise, but uses 176 bytes of flash.

        uint16_t factor = 1; //  1uA to 1000uA
        uint8_t i = 0;       //  1 byte loop reduces footprint
        bool result = false;
        do
        {
            if (1 * factor >= currentLSB_uA)
            {
                lsbI = 1 * factor * 1e-6;
                result = true;
            }
            else if (2 * factor >= currentLSB_uA)
            {
                lsbI = 2 * factor * 1e-6;
                result = true;
            }
            else if (5 * factor >= currentLSB_uA)
            {
                lsbI = 5 * factor * 1e-6;
                result = true;
            }
            else
            {
                factor *= 10;
                i++;
            }
        } while ((i < 4) && (!result)); //  factor < 10000

        if (result == false) //  not succeeded to normalize.
        {
            lsbI = 0;
            lastErrorString=INA226ErrorMap[INA226_ERR_NORMALIZE_FAILED];
            return false;
        }

#ifdef printdebug
        Serial.print("After scale current_LSB:\t");
        Serial.print(lsbI * 1e+6, 1);
        Serial.println(" uA / bit");
#endif
        // done
    }

    //  auto scale calibration if needed.
    uint32_t calib = round(0.00512 / (lsbI * shuntR));
    while (calib > 32767)
    {
        lsbI *= 2;
        calib >>= 1;
    }
    maxI = lsbI * 32768;

    lastErrorString=INA226ErrorMap[INA226_ERR_NONE];
    return writeWord(devAddr, INA226_REG_CAL, calib);
}

bool INA226::setAvaraging(INA226::Avaraging avgMask)
{
    readConfig();
    cfg.reg.avg=avgMask;
    return writeWord(devAddr,INA226_REG_CFG,cfg.value);
}

float INA226::getBusVoltage()
{
    //uint16_t iBusVoltage;
    //float fBusVoltage;

    uint16_t regVal=askForWord(devAddr, INA226_REG_BUS_VOLT);
    // printf("iBusVoltage = %04x\r\n", iBusVoltage);
    
    float voltage=regVal*1.25e-3;  //  fixed 1.25 mV

    // printf("Bus Voltage = %.2fV, ", fBusVoltage);
    return voltage;
}

float INA226::getShuntVoltage()
{
    uint16_t regVal=askForWord(devAddr, INA226_REG_SHUNT_VOLT);
    // printf("iShuntVoltage = %04x\r\n", iShuntVoltage);

    float voltage=regVal*2.5e-6;  //  fixed 2.5 uV

    // printf("Shunt Voltage = %.3fV\r\n", fShuntVoltage);
    return voltage;
}

float INA226::getCurrent()
{
    uint16_t regVal=askForWord(devAddr, INA226_REG_CURRENT);
    // Internally Calculated as Current = ((ShuntVoltage * CalibrationRegister) / 2048)
    float current=regVal*lsbI;
    // printf("Current = %.3fA\r\n", current);

    return current;
}

float INA226::getPower()
{
    uint16_t regVal=askForWord(devAddr, INA226_REG_POWER);
    // The Power Register LSB is internally programmed to equal 25 times the programmed value of the Current_LSB
    float power = regVal * lsbI * 25;

    // printf("Power = %.2fW\r\n", power);
    return power;
}

uint32_t INA226::getManufacturerID(void)
{
    return askForWord(devAddr, INA226_REG_MANUFACTURER_ID);
}

uint32_t INA226::getDieID(void)
{
    return askForWord(devAddr, INA226_REG_DIE_ID);
}

std::string INA226::getInfo(void)
{
    std::string info;
    char sRet[5];
    sprintf(sRet, "%04X", askForWord(devAddr, INA226_REG_MANUFACTURER_ID));
    info += "Manufacturer ID: 0x" + std::string(sRet) + "\n";
    sprintf(sRet, "%04X", askForWord(devAddr, INA226_REG_DIE_ID));
    info += "Die ID:          0x" + std::string(sRet) + "\n";
    sprintf(sRet, "%04X", askForWord(devAddr, INA226_REG_CFG));
    info += "Cfg register:    0x" + std::string(sRet) + "\n";
    sprintf(sRet, "%04X", askForWord(devAddr, INA226_REG_CAL));
    info += "Cal register:    0x" + std::string(sRet) + "\n";

    return info;
}