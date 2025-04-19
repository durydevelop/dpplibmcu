#include "dgpiochip.h"
#ifndef ARDUINO
#include <lgpio.h>
#include <derrorcodes.h>

DGpioChip::DGpioChip(int deviceIndex)
{
    gpioChipHandle=lgGpiochipOpen(deviceIndex);
    if (gpioChipHandle >= 0) {
        int ret=lgGpioGetChipInfo(gpioChipHandle, &cInfo);
        if (ret < 0) {
            gpioChipHandle=ret;
        }
    }
}

DGpioChip::~DGpioChip()
{
    if (gpioChipHandle >= 0) {
        lgGpiochipClose(gpioChipHandle);
    }
}

DGpioHandle DGpioChip::handle(void)
{
    return gpioChipHandle;
}

std::string DGpioChip::getLastError(void)
{
    if (gpioChipHandle < 0) {
        return getErrorCode(gpioChipHandle);
    }
    else {
        return "No error";
    }
}

bool DGpioChip::isReady(void)
{
    return gpioChipHandle >= 0;
}

std::string DGpioChip::getInfo(void)
{
    std::string info="Chip name="+std::string(cInfo.name)+" Label="+std::string(cInfo.label)+" I/O lines="+std::to_string(cInfo.lines)+" Handle="+std::to_string(gpioChipHandle);
    return info;
}
#endif