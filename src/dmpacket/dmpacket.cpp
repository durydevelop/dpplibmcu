#include "dmpacket.h"
/**
 * @brief DPacket is a buffer with a set of extended function to easy store and extract primitive data types.
 * Data type as integer (8, 16 bit) float, byte, word, dword can be easly stored and extracted.o
 * 
 * DPacket is developed with aim to easy send data between MCU, SBC and every device, over trasnsmission data channel like serial, I2C, SPI, CAN and whatever protocol.
 * From une side you can do something like:
 * packet.AddFloat(my_float);
 * and from the other side:
 * float my_float=packet.ExtractFloat();
 * You don't need to serialize and de-serialize bytes to reconstruct the value, DPacket do it for you.
 * Or you can simply use it as a buffer containing a vector of bytes to send raw data.
 * 
 * *\section caratteristiche_sec Caratteristiche
 * 
 */

#ifdef ARDUINO
	#define HIBYTE(w) highByte(w)
	#define LOBYTE(w) lowByte(w)
	#define WORD(hb,lb) word(hb,lb)
	#define LOWORD(dw) ((uint16_t)(dw))
    #define HIWORD(dw) ((uint16_t)(((uint32_t)(dw) >> 16) & 0xFFFF))
	#define DWORD_B(hb,next_hb,next_lb,lb) (((uint32_t)hb << 24) | ((uint32_t)next_hb << 16) | ((uint32_t)next_lb << 8) | lb)
#else
    //typedef uint8_t BYTE;
    //typedef uint16_t WORD;
    //typedef uint32_t DWORD;
    #include <sstream>
    #include <iomanip>
    #include <algorithm>
    #define bitRead(x, n) (((x) >> (n)) & 0x01)
    #define LOWORD(l) ((unsigned short)(l))
    #define HIWORD(l) ((unsigned short)(((unsigned int)(l) >> 16) & 0xFFFF))
    #define LOBYTE(w) ((char)(w&0x00FF))
    #define HIBYTE(w) ((char)((w>>8)&0x00FF))
    #define WORD(msb,lsb) ((msb << 8) | lsb)
    #define DWORD(msw,lsw) ((msw << 16) | lsw)
    #define DWORD_B(msb,next_msb,next_lsb,lsb) (msb << 24) | (next_msb << 16) | (next_lsb << 8) | lsb
#endif

/**
 * @brief Crea un pacchetto vuoto
 */
DMPacket::DMPacket() {
	Clear();
};

/**
 * @brief Crea un pacchetto eseguendo il parsing del buffer passato
 * @param Buff		->	buffer di bytes.
 */
DMPacket::DMPacket(const std::vector<uint8_t>& BuffVec) {
	SetBuffer(BuffVec);
};

/**
 * @brief Crea un pacchetto eseguendo il parsing del buffer passato
 * @param BuffVec	->  un std::vector<uint8_t>.
 */
DMPacket::DMPacket(uint8_t Buff[], uint16_t BuffLen) {
	SetBuffer(Buff,BuffLen);
};

//! Distruttore
DMPacket::~DMPacket() {
};

/**
 * @brief Resetta il pacchetto.
 * -Riporta la dimensione a HDR_SIZE.
 * -Azzera il contenuto.
 * -Azzera li ShiftIndex
 */
void DMPacket::Clear() {
	PacketBuff.resize(0);
	PacketBuff.shrink_to_fit();
    ShiftIndex=0;
}

//! @return la lunghezza in bytes del paccketto
uint8_t DMPacket::Size(void) {
	return(PacketBuff.size());
}

/**
 * @brief Copia Buff direttamente nel buffer del pacchetto.
 * 
  * @param Buff		->  buffer di bytes.
 * @param BuffSize	->	lunghezza del buffer (max 256).
 *
 **/
void DMPacket::SetBuffer(const uint8_t Buff[], uint16_t BuffSize) {
	// Set buffer size
	PacketBuff.resize(BuffSize);
	// Fill it
	//PacketBuff.assign(Buff,BuffLen);
	for (uint8_t ixB=0; ixB<BuffSize; ixB++) {
		PacketBuff[ixB]=Buff[ixB];
	}
    // Zeros shift index
    ShiftIndex=0;
}

void DMPacket::SetBuffer(const char Buff[], uint16_t BuffSize) {
    SetBuffer((uint8_t *) Buff,BuffSize);
}

/**
 * @brief Copia BuffVect direttamente nel buffer del pacchetto.
 * 
 * @param BuffVec	->  un std::vector<uint8_t>.
 * @return true 
 * @return false 
 */
void DMPacket::SetBuffer(const std::vector<uint8_t>& BuffVec) {
	PacketBuff=BuffVec;
    ShiftIndex=0;
}

//! @return un puntatore all'intero buffer del pacchetto.
uint8_t* DMPacket::BufferRaw() {
	return(PacketBuff.data());
}

//! @return Il riferimeto al buffer totale del pacchetto come vector.
std::vector<uint8_t>& DMPacket::Buffer() {
	return(PacketBuff);
}

/**
 * @brief 
 * 
 * @param Offset 
 * @param Len 
 * @return std::string 
 */
std::string DMPacket::ReadString(uint16_t Offset, uint16_t Lenght) {
    if (PacketBuff.empty()) {
        // Empy string
        return(std::string());
    }

    if ((Offset+Lenght) > PacketBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            return std::string();
        #endif
    }

	#ifdef ARDUINO
        std::string sData((char*)PacketBuff.data()+Lenght,PacketBuff.size());
    #else
	    std::string sData(PacketBuff.begin()+Offset,PacketBuff.end());
	#endif
	
	return(sData);
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di bytes (uint8_t)
 * 
 * @return un vector<uint8_t> contenente il payload
 */
std::vector<uint8_t> DMPacket::ReadBytes(uint16_t Offset, uint16_t Count)
{
    if ((Offset+Count) > PacketBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            // tronca
            Count=PacketBuff.size()-Offset;
        #endif
    }

    if (Count == 0) {
        Count=PacketBuff.size()-Offset;
    }
    
	std::vector<uint8_t> Data(Count);

//    #ifdef ARDUINO
        for (size_t ixP=Offset; ixP<PacketBuff.size(); ixP++) {
            Data[ixP-Offset]=PacketBuff[ixP];
        }
//    #else
//	    Data.assign(PacketBuff.begin()+Count,PacketBuff.end());
    //#endif

	#ifdef ARDUINO
	    return Data;
    #else
        return std::move(Data);
    #endif
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di bytes (uint8_t)
 * 
 * @param DataDest	->	Vettore di bytes in cui mettere i dati
 * @return false se i dati del pacchetto non sono di tipo byte o se è un pacchetto senza senza dati.
 */
uint16_t DMPacket::ReadBytes(std::vector<uint8_t>& Dest, uint16_t Offset, uint16_t Count)
{
    if ((Offset+Count) > PacketBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            // tronca
            Count=PacketBuff.size()-Offset;
        #endif
    }

    if (Count == 0) {
        Count=PacketBuff.size()-Offset;
    }
    
	Dest.resize(Count);

	//#ifdef ARDUINO
        for (size_t ixP=Offset; ixP<PacketBuff.size(); ixP++) {
            Dest[ixP-Offset]=PacketBuff[ixP];
        }
    //#else
    //    std::copy(PacketBuff.begin()+Count,PacketBuff.end(),Dest);
    //#endif

	return(Dest.size());
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di words (uint16_t)
 * 
 * @param Data	->	Vettore di words in cui mettere i dati
 * @return false se i dati del pacchetto non sono di tipo byte o se è un pacchetto senza senza dati.
 */
std::vector<uint16_t> DMPacket::ReadWords(uint16_t Offset, uint16_t Count)
{
	std::vector<uint16_t> Data;
    if ((Offset+(Count*2)) > PacketBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            // tronca all'utimo multiplo
            uint16_t FreeSpace=PacketBuff.size()-Offset;
            uint16_t FreeBlocks=FreeSpace/2;
            Count=FreeBlocks*2;
        #endif
    }
	Data.reserve(Count);

    #ifdef ARDUINO
        // Read 2 bytes at time
        for (uint16_t ixP=Count; ixP<PacketBuff.size(); ixP+=2) {
            Data[ixP-Count]=PacketBuff[ixP] | (PacketBuff[ixP+1] << 8);
        }
    #else
        // Read 2 bytes at time
        auto itr=PacketBuff.begin()+Offset;
        while(itr != PacketBuff.end()) {
            // Create word
            Data.emplace_back(*itr++ | (*itr++ << 8));
        }
    #endif

	return(Data);
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di double words (uint32_t)
 * 
 * @param Data	->	Vettore di words in cui mettere i dati
 * @return false se i dati del pacchetto non sono di tipo byte o se è un pacchetto senza senza dati.
 */
std::vector<uint32_t> DMPacket::ReadDWords(uint16_t Offset, uint16_t Count)
{
	std::vector<uint32_t> Data;
    if (Offset+(Count*4) > PacketBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            // tronca all'utimo multiplo
            uint16_t FreeSpace=PacketBuff.size()-Offset;
            uint16_t FreeBlocks=FreeSpace/4;
            Count=FreeBlocks*4;
        #endif
    }
	Data.reserve(Count);

    #ifdef ARDUINO
        // Read 4 bytes at time
        for (uint16_t ixP=Count; ixP<PacketBuff.size(); ixP+=4) {
            Data[ixP-Offset]=DWORD_B(PacketBuff[ixP],PacketBuff[ixP+1],PacketBuff[ixP+2],PacketBuff[ixP+3]);
        }
    #else
        // Read 4 bytes at time
        auto itr=PacketBuff.begin()+Offset;
        while(itr != PacketBuff.end()) {
            // Create double word
            Data.emplace_back((*itr++ << 24) | (*itr++ << 16) | (*itr++ << 8) | *itr++);
        }
    #endif

	return(Data);
}

std::string DMPacket::ToHexString(uint16_t Offset) {
    if (PacketBuff.empty()) {
        return std::string();
    }
    uint16_t MemSize=(PacketBuff.size()-Offset)*3; // 2 bytes + ':' for each byte
    char HexStr[MemSize];
    char *itr=&HexStr[0]; // iterator pointer
    //size_t ixP;
    for (size_t ixP=Offset; ixP<PacketBuff.size(); ixP++) {
        itr+=sprintf(itr,"%02X:",PacketBuff[ixP]);
    }
    return (std::string(HexStr,MemSize-1)); // (-1 because last byte have no ':' to the end)
}

std::string DMPacket::ToAsciiString(uint16_t Offset) {
    if (PacketBuff.empty()) {
        return std::string();
    }
    uint16_t MemSize=(PacketBuff.size()-Offset)*3;
    char AsciiStr[MemSize];
    char *itr=&AsciiStr[0]; // iterator pointer
    //size_t ixP;
    for (size_t ixP=Offset; ixP<PacketBuff.size(); ixP++) {
        itr+=sprintf(itr,"%c  ",isprint(PacketBuff[ixP]) ? PacketBuff[ixP] : '.');
    }
    return (std::string(AsciiStr,MemSize)); // (-1 because last byte have no ':' to the end)
}

/**
 * @brief Read a byte from data payload.
 * @param Index ->  Offset of byte to read in the payload.
 * @return 8 bit value.
 */
uint8_t DMPacket::ReadByte(uint16_t Offset)
{
    if (PacketBuff.empty()) {
        return 0;
    }

    if (Offset > PacketBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            // legge l'ultimo
            Offset=PacketBuff.size()-1;
            // TODO: oppure ritorna 0
        #endif
    }
    return(PacketBuff[Offset]);
}

/**
 * @brief Read a word from data payload.
 * @param Index ->  Offset of byte in the payload where to start reading.
 * @return 16 bit value.
 */
uint16_t DMPacket::ReadWord(uint16_t Offset)
{
    if (Offset+2 > PacketBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            return 0;
        #endif
    }
	return(WORD(PacketBuff[Offset],PacketBuff[Offset+1]));
}

/**
 * @brief Read a double-word from data payload.
 * @param Index ->  Offset of byte in the payload where to start reading.
 * @return 32 bit value.
 */
uint32_t DMPacket::ReadDWord(uint16_t Offset)
{
    if (Offset+4 > PacketBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            return 0;
        #endif
    }
	return(DWORD_B(PacketBuff[Offset],PacketBuff[Offset+1],PacketBuff[Offset+2],PacketBuff[Offset+3]));
}

/**
 * @brief Read a 16 bit signed integer from data payload.
 * @param Index ->  Offset of byte to read in the payload.
 * @return 16 bit integer value.
 */
int16_t DMPacket::ReadInt16(uint16_t Offset)
{
	return((int16_t)ReadWord(Offset));
}

/**
 * @brief Read a 8 bit signed integer from data payload.
 * @param Index ->  Offset of byte to read in the payload.
 * @return integer value.
 */
int8_t DMPacket::ReadInt8(uint16_t Offset)
{
	return((int8_t)ReadByte(Offset));
}

/**
 * @brief Read a 32 bit float from data.
 * @param Index ->  Number of the float to read (first is 1).
 * @return float value.
 */
float DMPacket::ReadFloat(uint16_t Offset)
{
	uint32_t DWord=ReadDWord(Offset);
	// Important cast from dword to float
	//return(*(float*)&DWord);
	//return(float(DWord));
    union {
        float ff;
        uint32_t ii;
    }d;
    d.ii=DWord;
    return d.ff;
}

/**
 * @brief Add a single byte to the packet.
 * N.B. DataType in header is not changed.
 * 
 * @param Byte  ->  byte to add.
 */
void DMPacket::PushByte(uint8_t Byte)
{
	PacketBuff.emplace_back(Byte);
}

/**
 * @brief Add a WORD (2 bytes) to the packet.
 * N.B. DataType in header is not changed.
 * 
 * @param Word  -> WORD to add.
 */
void DMPacket::PushWord(uint16_t Word)
{
	uint8_t Ix=PacketBuff.size();
	PacketBuff.resize(Ix+2);
	PacketBuff[Ix]=HIBYTE(Word);
	PacketBuff[Ix+1]=LOBYTE(Word);
}

/**
 * @brief Add a DWORD (4 bytes) to the packet.
 * N.B. DataType in header is not changed.
 * 
 * @param DWord ->  DWORD to add.
 */
void DMPacket::PushDWord(uint32_t DWord)
{
	uint8_t Ix=PacketBuff.size();
	PacketBuff.resize(Ix+4);
	PacketBuff[Ix]=HIBYTE(HIWORD(DWord));
	PacketBuff[Ix+1]=LOBYTE(HIWORD(DWord));
	PacketBuff[Ix+2]=HIBYTE(LOWORD(DWord));
	PacketBuff[Ix+3]=LOBYTE(LOWORD(DWord));
}

/**
 * @brief Add a 16 bit integer value to the packet.
 * N.B. DataType in header is not changed.
 * 
 * @param Int16 
 */
void DMPacket::PushInt16(int16_t Int16)
{
	PushWord(Int16);
}

/**
 * @brief Add a 32 bit floating point value to the packet.
 * N.B. DataType in header is not changed.
 * 
 * @param Float -> 32 bit float to add.
 */
void DMPacket::PushFloat(float Float)
{
	uint8_t Ix=PacketBuff.size();
	PacketBuff.resize(Ix+4);
	// Important cast from float to dword
	//uint32_t f=*(uint32_t*)&Float;
	//uint32_t f=float(Float);
    union {
        float ff;
        uint32_t ii;
    }d;
    d.ff=Float;
	PacketBuff[Ix]=HIBYTE(HIWORD(d.ii));
	PacketBuff[Ix+1]=LOBYTE(HIWORD(d.ii));
	PacketBuff[Ix+2]=HIBYTE(LOWORD(d.ii));
	PacketBuff[Ix+3]=LOBYTE(LOWORD(d.ii));
}

/**
 * @brief Add a string as a sequence of bytes to the packet.
 * N.B. DataType in header is not changed.
 * 
 * @param Str   ->  string to add.
 */
void DMPacket::PushString(std::string Str)
{
    #ifdef ARDUINO
        size_t currSize=PacketBuff.size();
	    PacketBuff.resize(currSize+Str.size());
        for (size_t ixP=currSize; ixP<PacketBuff.size(); ixP++) {
            PacketBuff[ixP]=Str[ixP-currSize];
        }
    #else
        const auto response_size = PacketBuff.size();
        PacketBuff.resize(response_size + Str.size());
        std::transform(Str.begin(), Str.end(), std::next(PacketBuff.begin(), response_size), [](uint8_t c) {
            return c;
        });
/*
        response.reserve(response.size() + headers.size());  // Optional
std::transform(headers.begin(), headers.end(), std::back_inserter(response),
    [](unsigned char c) { return std::byte{c}; }              // or `encode(c)`
);
*/
    #endif
}

void DMPacket::PushData(const std::vector<uint8_t>& BuffVec) {
    PacketBuff.reserve(PacketBuff.size() + BuffVec.size());
    for(uint16_t ixV=0; ixV<BuffVec.size(); ixV++) {
        PacketBuff.emplace_back(BuffVec[ixV]);
    }
    /*
    for (uint8_t b : BuffVec) {
        PacketBuff.emplace_back(b);
    }
    */
}

void DMPacket::pushData(const uint8_t buff[], const uint16_t buffSize) {
    uint16_t startByte=PacketBuff.size();
    PacketBuff.resize(startByte + buffSize);
    for (uint8_t ixB=startByte; ixB<buffSize; ixB++) {
		PacketBuff[ixB]=*buff++;
	}
}

/**
 * @brief Pop a byte from begin of the buffer.
 * It does not really shift the buffer, use a shift index to set the current pop posision
 * (shifting a vector can be an expensive operation on vector).
 * 
 * @return uint8_t 
 */
uint8_t DMPacket::ShiftByte(void) {
    if (PacketBuff.size() > 0) {
        //return PacketBuff[ShiftIndex++];
        uint8_t data=PacketBuff[ShiftIndex];
        ShiftIndex++;
        return data;
    }
    return 0;
}

std::string DMPacket::ShiftString(uint16_t Lenght) {
    if (PacketBuff.size() > 0) {
        std::string s=ReadString(ShiftIndex,Lenght);
        ShiftIndex+=s.size();
        // TODO: needs?
        //if (ShiftIndex >= PacketBuff.size()) {
        //    ShiftIndex=PacketBuff.size()-1;
        //}
        return s;
    }
    return std::string();
}
