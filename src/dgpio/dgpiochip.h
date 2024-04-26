#ifndef DGpioChip_H
#define DGpioChip_H

#include <string>
#include <lgpio.h>

typedef int DGpioHandle;

class DGpioChip
{
    public:
        DGpioChip(int deviceIndex);
        ~DGpioChip();
        DGpioHandle handle(void);
        std::string getLastError(void);
        bool isReady(void);
        std::string getInfo();

    private:
        DGpioHandle gpioChipHandle;
        lgChipInfo_t cInfo;
};

#endif