#include "DMPacket.h"
/**
 * @brief 
 * 
 * *\section caratteristiche_sec Caratteristiche
* Struttura pacchetto dati

Header size     = 4
Max packet size = 256
Max data size   = 256 - 4 = 252

         +-------------------------------------------------------+-----------------------------------+
         |                         HEADER                        |                DATA               |
+--------+-------------+-------------+-------------+-------------+-------------+---+---+-------------+
| Offset |      0      |      1      |      2      |      3      |      4      |...|...|     254     |
+--------+-------------+-------------+-------------+-------------+-------------+---+---+-------------+
| Type   | Start Byte  |  Cmd Code   |  Data Type  | Data Length |    Data     |...|...|    Data     |
+--------+-------------+-------------+-------------+-------------+-------------+---+---+-------------+
| Size   |   1 byte    |   1 byte    |   1 byte    |   1 byte    |   1 byte    |...|...|   1 byte    |
+--------+-------------+-------------+-------------+-------------+-------------+---+---+-------------+
| Value  |     0x01    |  0x00-0xFF  |  DDataType  |  0x00-0xFF  |  0x00-0xFC  |...|...|  0x00-0xFF  |
+--------+-------------+-------------+-------------+-------------+-------------+---+---+-------------+

 * 
 */
/*
union u_conv{
	uint8_t Byte[4];
	uint16_t Word[2];
	uint32_t DWord;
	float Float;
	short int ShortInt[2];
	unsigned short int UShortInt[2];
	int Int;
	unsigned int UInt;
}conv;
*/
#ifdef ARDUINO
	#define HIBYTE(w) highByte(w)
	#define LOBYTE(w) lowByte(w)
	#define WORD(hb,lb) word(hb,lb)
	#define LOWORD(dw) ((uint16_t)(dw))
    #define HIWORD(dw) ((uint16_t)(((uint32_t)(dw) >> 16) & 0xFFFF))
	#define DWORD_B(hb,next_hb,next_lb,lb) (((uint32_t)hb << 24) | ((uint32_t)next_hb << 16) | ((uint32_t)next_lb << 8) | lb)
#else
    typedef uint8_t BYTE;
    typedef uint16_t WORD;
    typedef uint32_t DWORD;

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
	Reset();
};

/**
 * @brief Crea un pacchetto eseguendo il parsing del buffer passato
 * @param Buff		->	buffer di bytes.
 * @param BuffLen	->	lunghezza del buffer.
 */
DMPacket::DMPacket(uint8_t Buff[], uint8_t BuffLen) {
	Set(Buff,BuffLen);
};

/**
 * @brief Costruisce il pacchetto dai singoli elementi
 * @param Cmd		->	un byte contenente il Comando.
 * @param DataType	->	tipo di dati associati, può essere uno dei valori di DDataType.
 * @param DataSize	->	lunghezza del buffer dati.
 * @param DataBuff	->  buffer dati.
 */
DMPacket::DMPacket(uint8_t Cmd, DDataType DataType, uint8_t DataSize, uint8_t DataBuff[]) {
    Set(Cmd,DataType,DataSize,DataBuff);
};

/**
 * @brief Costruisce un paccketto con comando e data type senza dati
 * 
 * @param Cmd		->	un byte contenente il Comando.
 * @param DataType	->	tipo di dati associati, può essere uno dei valori di DDataType.
 */
DMPacket::DMPacket(uint8_t Cmd, DDataType DataType)
{
	Set(Cmd,DataType);
}

/**
 * @brief Costruisce un pacchetto con comando e senza dati
 * 
 * @param Cmd		->	un byte contenente il Comando.
 */
DMPacket::DMPacket(uint8_t Cmd)
{
	Set(Cmd);
}

//! Distruttore
DMPacket::~DMPacket() {
};

/**
 * @brief Resetta il pacchetto.
 * -Riporta la dimensione a HDR_SIZE.
 * -Azzera il contenuto.
 */
void DMPacket::Reset() {
	PacketBuff.resize(HDR_SIZE,0);
	PacketBuff.shrink_to_fit();
	Valid=false;
}

//! @return true se il pacchetto contiene dati validi altrimeti false
bool DMPacket::IsValid(void) {
	return(Valid);
}

//! @return la lunghezza in bytes del paccketto
uint8_t DMPacket::Size(void) {
	return(PacketBuff.size());
}

//! Controlla l'integrità del pacchetto.
bool DMPacket::ValidatePacket(void) {
	Valid=false;

	// Start byte
	if (PacketBuff[OFFSET_START_BYTE] != START_BYTE) {
		return false;
	}

	// Data type check
	if (//PacketBuff[OFFSET_DATA_TYPE] != DATA_TYPE_NO_DATA	&&
		PacketBuff[OFFSET_DATA_TYPE] != DATA_TYPE_BYTE		&&
		PacketBuff[OFFSET_DATA_TYPE] != DATA_TYPE_WORD		&&
		PacketBuff[OFFSET_DATA_TYPE] != DATA_TYPE_DWORD		&&
		PacketBuff[OFFSET_DATA_TYPE] != DATA_TYPE_STRING
		//PacketBuff[OFFSET_DATA_TYPE] != DATA_TYPE_ANY
	) {
		return false;
	}

	// Data lenght check
	if (PacketBuff.size()-HDR_SIZE == PacketBuff[OFFSET_DATA_SIZE]) {
		Valid=true;
	}

	return(Valid);
}

//! Genera il pacchetto dal buffer passato poi esegue ValidatePacket().
/**
* @param Buff		->  buffer di bytes.
* @param BuffSize	->	lunghezza del buffer (max 256).
*
* Se il pacchetto è valido la variabile Validate diventa true.
**/
bool DMPacket::Set(uint8_t Buff[], uint16_t BuffSize) {
	Valid=false;

	if (BuffSize < MIN_PACKET_SIZE || BuffSize > MAX_PACKET_SIZE) {
		Reset();
		return false;
	}

	// Set buffer size
	PacketBuff.resize(BuffSize);
	// Fill it
	//PacketBuff.assign(Buff,BuffLen);
	for (uint8_t ixB=0; ixB<BuffSize; ixB++) {
		PacketBuff[ixB]=Buff[ixB];
	}

	return(ValidatePacket());
}

/** Sequenzia il pacchetto con i singoli elementi
* @param Cmd		->	un byte contenente il Comando.
* @param DataType	->	tipo di dati associati, può essere uno dei valori di DDataType.
* @param DataSize	->	lunghezza del buffer dati.
* @param DataBuff	->  buffer dati.
**/
bool DMPacket::Set(uint8_t Cmd, DDataType DataType, uint8_t DataSize, uint8_t DataBuff[]) {
	Reset();
	PacketBuff[OFFSET_START_BYTE]=START_BYTE;
	PacketBuff[OFFSET_CMD]=Cmd;
	SetData(DataType,DataSize,DataBuff);
	return(IsValid());
}

//! Sequenzia un pacchetto con comando e tipo di dato contenuto (nessun dato)
void DMPacket::Set(uint8_t Cmd, DDataType DataType) {
	Reset();
	PacketBuff[OFFSET_START_BYTE]=START_BYTE;
	PacketBuff[OFFSET_CMD]=Cmd;
	PacketBuff[OFFSET_DATA_TYPE]=DataType;
}

//! Sequenzia un paccketto con comando (tipo di dati DATA_TYPE_BYTE e nessun dato)
void DMPacket::Set(uint8_t Cmd) {
	Reset();
	PacketBuff[OFFSET_START_BYTE]=START_BYTE;
	PacketBuff[OFFSET_CMD]=Cmd;
	// PacketBuff[OFFSET_DATA_TYPE] already 0 (DATA_TYPE_BYTE)
	// PacketBuff[OFFSET_DATA_SIZE] already 0
}

/**
 * @brief Sequenzia i dati del pacchetto, i dati presenti vengono eliminati e sovrascritti.
 * N.B. Se la lunghezza dei dati supera MAX_DATA_SIZE il buffer viene troncato.
 * @param DataType	->	tipo di dati associati, può essere uno dei valori di DDataType.
 * @param DataSize	->	lunghezza del buffer dati.
 * @param DataBuff	->  buffer dati.
 */
void DMPacket::SetData(DDataType DataType, uint8_t DataSize, uint8_t DataBuff[]) {
	if (DataSize > MAX_DATA_SIZE) {
		DataSize=MAX_DATA_SIZE;
	}

	PacketBuff[OFFSET_DATA_TYPE]=DataType;
	PacketBuff[OFFSET_DATA_SIZE]=DataSize;

	PacketBuff.resize(HDR_SIZE+DataSize);
	/*
	uint8_t ixD=0;
	while(ixD < DataSize) {
		PacketBuff[ixD+HDR_SIZE]=DataBuff[ixD];
		ixD++;
	}
	*/

	for (uint8_t ixD=0; ixD<DataSize; ixD++) {
		PacketBuff[ixD+HDR_SIZE]=DataBuff[ixD];
	}
}

//! @return true se sono presenti realmente dati nel payload dati.
bool DMPacket::HasData(void)
{
	return(PacketBuff.size() > HDR_SIZE);
}

//! @return un puntatore al buffer generale del pacchetto.
uint8_t* DMPacket::Get() {
	return(PacketBuff.data());
}

//! @return il byte contenente il comando del pacchetto.
uint8_t DMPacket::GetCmd() {
	return(PacketBuff[OFFSET_CMD]);
}

//! @return il valore del byte che rappresenta il tipo di dati contenuti nel pacchetto. Può essere uno dei valori di DDataType.
uint8_t DMPacket::GetDataType() {
	return(PacketBuff[OFFSET_DATA_TYPE]);
}

//! @return il valore del byte che rappresenta la lunghezza in bytes del payload dati.
uint8_t DMPacket::GetDataSize() {
	return(PacketBuff[OFFSET_DATA_SIZE]);
}

//! @return La lunghezza reale payload dati.
uint8_t DMPacket::GetRealDataSize() {
    return(PacketBuff.size()-HDR_SIZE);
}

/**
 * @brief Calcola quanti dati di tipo DDataType sono dichiarati nel pacchetto.
 * Se i dati sono di tipo word e il byte OFFSET_DATA_SIZE contiene 4, ci sono 2 dati.
 * N.B. Se il pacchetto non è stato sequenziato correttamente potrebbe non rispecchiare il reale contenuto dei dati.
 * @return il nr di dati presenti nel pacchetto.
 */
uint8_t DMPacket::GetDataCount() {
	if (PacketBuff[OFFSET_DATA_TYPE] == DATA_TYPE_STRING) {
		return(PacketBuff[OFFSET_DATA_SIZE]);
	}
	else {
		// Calculate using data type size
		return(PacketBuff[OFFSET_DATA_SIZE]/(PacketBuff[OFFSET_DATA_TYPE]));
	}
}

//! @return un puntatore al buffer dati del pacchetto
uint8_t* DMPacket::GetData() {
	/* TODO
	vector<uint8_t>::iterator itr=PacketBuff.begin()+OFFSET_DATA;
	return(&(*itr));
	*/
	return(PacketBuff.data()+OFFSET_DATA);
}

//! @return la stringa contenuta nel data buffet se il DataType è DATA_TYPE_STRING altimenti una stringa vuota
std::string DMPacket::GetDataAsString() {
	if (PacketBuff[OFFSET_DATA_TYPE] != DATA_TYPE_STRING) {
		return(std::string());
	}
	/* TODO
	std::string sData(PacketBuff.begin()+OFFSET_DATA,PacketBuff.end());
	return(sData);
	*/
	std::string sData((char*)GetData(),PacketBuff.size());
	return(sData);
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di bytes (uint8_t)
 * 
 * @return un vector<uint8_t> contenente il payload
 */
std::vector<uint8_t> DMPacket::GetDataAsBytes(void)
{
	std::vector<uint8_t> Data;
	Data.reserve(GetRealDataSize());
	//Data.assign(PacketBuff.begin()+HDR_SIZE,PacketBuff.end());
	for (size_t ixP=HDR_SIZE; ixP<PacketBuff.size(); ixP++) {
		Data[ixP-HDR_SIZE]=PacketBuff[ixP];
	}
	return(Data);
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di bytes (uint8_t)
 * 
 * @param DataDest	->	Vettore di bytes in cui mettere i dati
 * @return false se i dati del pacchetto non sono di tipo byte o se è un pacchetto senza senza dati.
 */
uint8_t DMPacket::GetDataAsBytes(std::vector<uint8_t>& DataDest)
{
	DataDest.reserve(GetRealDataSize());
	// TODO Data.assign(PacketBuff.begin()+HDR_SIZE,PacketBuff.end());
	for (size_t ixP=HDR_SIZE; ixP<PacketBuff.size(); ixP++) {
		DataDest[ixP-HDR_SIZE]=PacketBuff[ixP];
	}

	return(DataDest.size());
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di words (uint16_t)
 * 
 * @param Data	->	Vettore di words in cui mettere i dati
 * @return false se i dati del pacchetto non sono di tipo byte o se è un pacchetto senza senza dati.
 */
std::vector<uint16_t> DMPacket::GetDataAsWords(void)
{
	std::vector<uint16_t> Data;
	Data.reserve(GetRealDataSize() / DATA_TYPE_WORD);
	/* TODO
	// Read 2 bytes at time
	auto itr=PacketBuff.begin()+HDR_SIZE;
	while(itr != PacketBuff.end()) {
		// Create word
		Data.emplace_back(*itr++ | (*itr++ << 8));
	}
	*/
	size_t ixP=HDR_SIZE;
	do {
		// Read 2 bytes at time
		Data[ixP-HDR_SIZE]=PacketBuff[ixP] | (PacketBuff[ixP+1] << 8);
		ixP+=2;
	} while (ixP < GetRealDataSize());

	return(Data);
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di double words (uint32_t)
 * 
 * @param Data	->	Vettore di words in cui mettere i dati
 * @return false se i dati del pacchetto non sono di tipo byte o se è un pacchetto senza senza dati.
 */
std::vector<uint32_t> DMPacket::GetDataAsDWords(void)
{
	std::vector<uint32_t> Data;
	Data.reserve(GetRealDataSize() / DATA_TYPE_DWORD);
	/* TODO
	// Read 4 bytes at time
	auto itr=PacketBuff.begin()+HDR_SIZE;
	while(itr != PacketBuff.end()) {
		// Create double word
		Data.emplace_back((*itr++ << 24) | (*itr++ << 16) | (*itr++ << 8) | *itr++);
	}
	*/
	size_t ixP=HDR_SIZE;
	do {
		// Read 4 bytes at time
		Data[ixP-HDR_SIZE]=DWORD_B(PacketBuff[ixP],PacketBuff[ixP+1],PacketBuff[ixP+2],PacketBuff[ixP+3]);
		ixP+=4;
	} while (ixP < GetRealDataSize());

	return(Data);
}

/**
 * @brief Read a byte from data.
 * @param Index ->  Number of the byte to read (first is 1).
 * @return 8 bit value.
 */
uint8_t DMPacket::ReadByte(uint8_t Index)
{
    return(PacketBuff[OFFSET_DATA+Index]);
}

/**
 * @brief Read a word from data.
 * @param Index ->  Number of the word to read (first is 1).
 * @return 16 bit value.
 */
uint16_t DMPacket::ReadWord(uint8_t Index)
{
	//return(PacketBuff[OFFSET_DATA+WordNumber-1] | PacketBuff[OFFSET_DATA+WordNumber] << 8);
	uint8_t Ix=OFFSET_DATA+(Index*2);
	return(WORD(PacketBuff[Ix],PacketBuff[Ix+1]));
}

/**
 * @brief Read a double-word data.
 * @param Index ->  Number of the doublw-word to read (first is 1).
 * @return 32 bit value.
 */
uint32_t DMPacket::ReadDWord(uint8_t Index)
{
	uint8_t Ix=OFFSET_DATA+(Index*4);
	return(DWORD_B(PacketBuff[Ix],PacketBuff[Ix+1],PacketBuff[Ix+2],PacketBuff[Ix+3]));
}

/**
 * @brief Read a 16 bit signed integer from data.
 * @param Index ->  Number of the integer to read (first is 1).
 * @return integer value.
 */
int16_t DMPacket::ReadInt16(uint8_t Index)
{
	uint8_t Ix=OFFSET_DATA+(Index*2);
	return(WORD(PacketBuff[Ix],PacketBuff[Ix+1]));
}

/**
 * @brief Read a 8 bit signed integer from data.
 * @param Index ->  Number of the integer to read (first is 1).
 * @return integer value.
 */
int8_t DMPacket::ReadInt8(uint8_t Index)
{
	return(PacketBuff[OFFSET_DATA+Index]);
}

/**
 * @brief Read a 32 bit float from data.
 * @param Index ->  Number of the float to read (first is 1).
 * @return float value.
 */
float DMPacket::ReadFloat(uint8_t Index)
{
	uint8_t Ix=OFFSET_DATA+(Index*4);
	uint32_t DWord=DWORD_B(PacketBuff[Ix],PacketBuff[Ix+1],PacketBuff[Ix+2],PacketBuff[Ix+3]);
	// Important cast from dword to float
	//return(*(float*)&DWord);
	return(float(DWord));
}

void DMPacket::AddByte(uint8_t Byte)
{
	//PacketBuff.resize(PacketBuff.size()+1);
	//PacketBuff[PacketBuff.size()-1]=Byte;
	PacketBuff.emplace_back(Byte);
	CalculateSize();
}

void DMPacket::AddWord(uint16_t Word)
{
	uint8_t Ix=PacketBuff.size();
	PacketBuff.resize(Ix+2);
	PacketBuff[Ix]=HIBYTE(Word);
	PacketBuff[Ix+1]=LOBYTE(Word);
	CalculateSize();
}

void DMPacket::AddDWord(uint32_t DWord)
{
	uint8_t Ix=PacketBuff.size();
	PacketBuff.resize(Ix+4);
	PacketBuff[Ix]=HIBYTE(HIWORD(DWord));
	PacketBuff[Ix+1]=LOBYTE(HIWORD(DWord));
	PacketBuff[Ix+2]=HIBYTE(LOWORD(DWord));
	PacketBuff[Ix+3]=LOBYTE(LOWORD(DWord));
	CalculateSize();
}

void DMPacket::AddInt(int Int)
{
	AddWord(Int);
	PacketBuff[OFFSET_DATA_SIZE]=PacketBuff.size()-OFFSET_DATA;
}

void DMPacket::AddFloat(float Float)
{
	uint8_t Ix=PacketBuff.size();
	PacketBuff.resize(Ix+4);
	// Important cast from float to dword
	//uint32_t f=*(uint32_t*)&Float;
	uint32_t f=float(Float);
	PacketBuff[Ix]=HIBYTE(HIWORD(f));
	PacketBuff[Ix+1]=LOBYTE(HIWORD(f));
	PacketBuff[Ix+2]=HIBYTE(LOWORD(f));
	PacketBuff[Ix+3]=LOBYTE(LOWORD(f));
	CalculateSize();
}

void DMPacket::AddString(std::string Str)
{
	PacketBuff.resize(PacketBuff.size()+Str.size());
	for (uint8_t ixD=0; ixD<Str.size(); ixD++) {
		PacketBuff[ixD+HDR_SIZE]=Str[ixD];
	}
	CalculateSize();
}

void DMPacket::CalculateSize(void)
{
	PacketBuff[OFFSET_DATA_SIZE]=PacketBuff.size()-OFFSET_DATA;
}
