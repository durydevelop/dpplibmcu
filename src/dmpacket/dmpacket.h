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
    	std::vector<uint8_t>PacketBuff;
        uint16_t ShiftIndex;
    public:
        DMPacket();
        DMPacket(uint8_t Buff[], uint16_t BuffLen);
        DMPacket(const std::vector<uint8_t>& BuffVec);
        ~DMPacket();

        void Clear();
        uint8_t Size(void);
        uint8_t* BufferRaw(void);
        std::vector<uint8_t>& Buffer(void);

        void SetBuffer(const uint8_t Buff[], uint16_t BuffLen);
        void SetBuffer(const char Buff[], uint16_t BuffLen);
        void SetBuffer(const std::vector<uint8_t>& BuffVec);

        uint8_t ReadByte(uint16_t Offset);
        uint16_t ReadWord(uint16_t Offset);
        uint32_t ReadDWord(uint16_t Offset);
        int8_t ReadInt8(uint16_t Offset);
        int16_t ReadInt16(uint16_t Offset);
        float ReadFloat(uint16_t Offset);
        std::vector<uint8_t> ReadBytes(uint16_t Offset, uint16_t Count = 0);
        uint16_t ReadBytes(std::vector<uint8_t>& Dest, uint16_t Offset, uint16_t Count = 0);
        std::vector<uint16_t> ReadWords(uint16_t Offset, uint16_t Count = 0);
        std::vector<uint32_t> ReadDWords(uint16_t Offset, uint16_t Count = 0);
        std::string ReadString(uint16_t Offset = 0, uint16_t Lenght = 0);

        /*
        // TODO
        void WriteByte(uint8_t Byte, uint16_t Offset);
        void WriteWord(uint16_t Word, uint16_t Offset);
        void WriteDWord(uint32_t DWord, uint16_t Offset);
        void WriteInt16(int16_t Int, uint16_t Offset);
        void WriteFloat(float Float, uint16_t Offset);
        void WriteString(std::string Str, uint16_t Offset);
*/

        void PushByte(uint8_t Byte);
        void PushWord(uint16_t Word);
        void PushDWord(uint32_t DWord);
        void PushInt16(int16_t Int);
        void PushFloat(float Float);
        void PushData(const std::vector<uint8_t>& BuffVec);
        void PushString(std::string Str);

        uint8_t ShiftByte(void);
        std::string ShiftString(uint16_t Lenght = 0);

        std::string ToHexString(uint16_t Offset = 0);
        std::string ToAsciiString(uint16_t Offset = 0);

};

#endif // DMPACKET_H
