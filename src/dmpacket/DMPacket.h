#ifndef DMPACKET_H
#define DMPACKET_H

#ifdef ARDUINO
	// Arduino inlcudes
    #if ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "wiring.h"
        #include "WProgram.h"
    #endif
#endif

#include <vector>
#include <string>

class DMPacket {
	private:
    	bool ValidatePacket(void);

    	std::vector<uint8_t>PacketBuff;
    	bool Valid;
    public:
        //! Packet offsets
        enum DOffset   { OFFSET_START_BYTE = 0, // Offset to byte: start
                         OFFSET_CMD,            // Offset to byte: command
                         OFFSET_DATA_TYPE,      // Offset to byte: type of data
                         OFFSET_DATA_SIZE,      // Offset to byte: size of data in bytes
                         OFFSET_DATA            // Offset to byte: start of data
        };
        //! Packet data types
        enum DDataType { DATA_TYPE_BYTE     = 1,    // Packet contains data as bytes
                         DATA_TYPE_WORD     = 2,    // Packet contains data as words
                         DATA_TYPE_DWORD    = 4,    // Packet contains data as double-words
                         DATA_TYPE_STRING   = 5     // Packet contains data as array of char
        };
        // Packet sizes
        static const uint8_t HDR_SIZE=OFFSET_DATA;
        static const uint16_t MAX_PACKET_SIZE=256;
        static const uint8_t MIN_PACKET_SIZE=HDR_SIZE;
        static const uint8_t MAX_DATA_SIZE=MAX_PACKET_SIZE-HDR_SIZE;
        // Start byte
        static const uint8_t START_BYTE=0x01;

        DMPacket();
        DMPacket(uint8_t Buff[], uint8_t BuffLen);
        DMPacket(uint8_t Cmd, DDataType DataType, uint8_t DataSize, uint8_t DataBuff[]);
        DMPacket(uint8_t Cmd, DDataType DataType);
        DMPacket(uint8_t Cmd);
        ~DMPacket();

        void Reset();
        bool IsValid(void);
        bool Set(uint8_t Buff[], uint16_t BuffLen);
        bool Set(uint8_t Cmd, DDataType DataType, uint8_t DataSize, uint8_t DataBuff[]);
        void Set(uint8_t Cmd, DDataType DataType);
        void Set(uint8_t Cmd);
        void SetData(DDataType DataType, uint8_t DataSize, uint8_t DataBuff[]);
        bool HasData(void);
        uint8_t Size(void);
        uint8_t* Get(void);

        uint8_t GetCmd(void);
        uint8_t GetDataType(void);
        uint8_t GetDataSize(void);
        uint8_t GetRealDataSize(void);
        uint8_t GetDataCount(void);
        uint8_t* GetData(void);
        std::string GetDataAsString();
        std::vector<uint8_t> GetDataAsBytes(void);
        uint8_t GetDataAsBytes(std::vector<uint8_t>& DataDest);
        std::vector<uint16_t> GetDataAsWords(void);
        std::vector<uint32_t> GetDataAsDWords(void);

        uint8_t ReadByte(uint8_t Index);
        uint16_t ReadWord(uint8_t Index);
        uint32_t ReadDWord(uint8_t Index);

        int8_t ReadInt8(uint8_t Index);
        int16_t ReadInt16(uint8_t Index);
        float ReadFloat(uint8_t Index);

        void AddByte(uint8_t Byte);
        void AddWord(uint16_t Word);
        void AddDWord(uint32_t DWord);
        void AddInt(int Int);
        void AddFloat(float Float);
        void AddString(std::string Str);

    private:
        void CalculateSize(void);
};

#endif // DMPACKET_H
