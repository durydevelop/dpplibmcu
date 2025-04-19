#ifndef DGpioChip_H
#define DGpioChip_H

#ifndef ARDUINO
    #include <string>
    #include <lgpio.h>

    typedef int DGpioHandle;

    class DGpioChip
    {
        public:
            DGpioChip(int deviceIndex = 0);
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

#endif