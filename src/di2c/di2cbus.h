#ifndef DI2CBus_H
#define DI2CBus_H

#include <cstdint>
#include <string>
#include <vector>

typedef int DI2CBusHandle;

class DI2CBus {
    public:
        enum DScanMode { SCAN_MODE_READ, SCAN_MODE_WRITE };
        struct DI2CAdaper {
            int nr;
            std::string name;
            std::string funcs;
            std::string algo;
        };

        DI2CBus(uint8_t busID);
        ~DI2CBus();

        int scanForDevices(DScanMode scanMode = SCAN_MODE_READ, uint8_t startAddr = 0x08, uint8_t endAddr = 0x77);
        DI2CBusHandle handle(void);
        bool isReady(void);
        std::string getLastError(void);
        std::string getInfo(void);

        static int openI2CBus(int busID);
        static int scanI2CBusses(std::vector<DI2CAdaper> &resultList);

        std::vector<uint8_t> devices;

    private:
        std::string busName;
        uint8_t devAddr;
        DI2CBusHandle busHandle;
        std::string lastErrorString;
        
};

#endif