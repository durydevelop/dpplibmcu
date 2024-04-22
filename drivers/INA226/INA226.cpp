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
 */

#include "INA226.h"

INA226::INA226(uint8_t deviceAddr, DI2CBusHandle i2cBusHandle) : DI2CMaster(i2cBusHandle)
{
    devAddr=deviceAddr;
}

INA226::~INA226()
{

}

bool INA226::begin(void)
{
	bool ret=writeWord(devAddr,INA226_CFG_REG, 0x8000); // Reset
	ret&=writeWord(devAddr,INA226_CFG_REG, 0x4527);     // Average over 16 Samples
	ret&=writeWord(devAddr, INA226_CAL_REG, 1024);      // 1A, 0.100Ohm Resistor
    return ret;
}

float INA226::voltage()
{
	uint16_t iBusVoltage;
	float fBusVoltage;

	iBusVoltage = askForWord(devAddr,INA226_BUS_VOLT_REG);
	//printf("iBusVoltage = %04x\r\n", iBusVoltage);
	fBusVoltage = (iBusVoltage) * 0.00125;
	//printf("Bus Voltage = %.2fV, ", fBusVoltage);

	return (fBusVoltage);
}

float INA226::current()
{
	int16_t iCurrent;
	float fCurrent;

	iCurrent = askForWord(devAddr,INA226_CURRENT_REG);
	// Internally Calculated as Current = ((ShuntVoltage * CalibrationRegister) / 2048)
	fCurrent = iCurrent * 0.0005;
	//printf("Current = %.3fA\r\n", fCurrent);

	return (fCurrent);
}

float INA226::power()
{
	int16_t iPower;
	float fPower;

	iPower = askForWord(devAddr,INA226_POWER_REG);
	// The Power Register LSB is internally programmed to equal 25 times the programmed value of the Current_LSB
	fPower = iPower * 0.0125;

	//printf("Power = %.2fW\r\n", fPower);
	return (fPower);
}

std::string INA226::getInfo(void)
{
    std::string info;
    char sRet[5];
    sprintf(sRet,"%04X",askForWord(devAddr,INA226_MANUFACTURER_ID));
    info+="Manufacturer ID:        0x"+std::string(sRet)+"\n";
    sprintf(sRet,"%04X",askForWord(devAddr,INA226_DIE_ID));
    info+="Die ID Register:        0x"+std::string(sRet)+"\n";
    sprintf(sRet,"%04X",askForWord(devAddr,INA226_CFG_REG));
    info+="Configuration Register: 0x"+std::string(sRet)+"\n";

    return info;
}