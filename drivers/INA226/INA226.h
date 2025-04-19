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

#ifndef INA226_H
#define INA226_H

#define INA226_SLAVE_ADDRESS	0x40
#define INA226_CFG_REG		0x00
#define INA226_SHUNT_VOLT_REG	0x01
#define INA226_BUS_VOLT_REG	0x02
#define INA226_POWER_REG	0x03
#define INA226_CURRENT_REG	0x04
#define INA226_CAL_REG		0x05
#define INA226_MASKEN_REG	0x06
#define INA226_ALERT_LMT_REG	0x07
#define INA226_MANUFACTURER_ID	0xFE
#define INA226_DIE_ID		0xFF

#include <di2cmaster>

class INA226 : public DI2CMaster {
    public:
        INA226(uint8_t deviceAddr, DI2CBusHandle i2cBusHandle);
        ~INA226();
        bool begin(void);
        bool isReady(void);
        float voltage(void);
        float current(void);
        float power(void);

        std::string getInfo(void);

    private:
        uint8_t devAddr;
};

#endif
