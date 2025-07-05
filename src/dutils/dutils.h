#ifndef DUtils_H
#define DUtils_H

#ifndef ARDUINO
    void delay(unsigned long ms);
    void delayMicroseconds(unsigned long us);
    unsigned long millis(void);
    unsigned long micros(void);

    //#define mapValue(x,in_min,in_max,out_min,out_max) (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min
    //template <typename T>
    //T mapValue(T x, T in_min, T in_max, T out_min, T out_max) {
    //    return(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    //};
    /*
    long map(long x, long in_min, long in_max, long out_min, long out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
    */

/*@todo
	#define HIBYTE(w) highByte(w)
	#define LOBYTE(w) lowByte(w)k
	#define WORD(hb,lb) word(hb,lb)
	#define LOWORD(dw) ((uint16_t)(dw))
    #define HIWORD(dw) ((uint16_t)(((uint32_t)(dw) >> 16) & 0xFFFF))
	#define DWORD_B(hb,next_hb,next_lb,lb) (((uint32_t)hb << 24) | ((uint32_t)next_hb << 16) | ((uint32_t)next_lb << 8) | lb)
*/
#endif

/* @todo
#define bitRead(x, n) (((x) >> (n)) & 0x01)
#define LOWORD(l) ((unsigned short)(l))
#define HIWORD(l) ((unsigned short)(((unsigned int)(l) >> 16) & 0xFFFF))
#define LOBYTE(w) ((uint8_t)(w&0x00FF))
#define HIBYTE(w) ((uint8_t)((w>>8)&0x00FF))
#define WORD(msb,lsb) ((msb << 8) | lsb)
#define DWORD(msw,lsw) ((msw << 16) | lsw)
#define DWORD_B(msb,next_msb,next_lsb,lsb) (msb << 24) | (next_msb << 16) | (next_lsb << 8) | lsb
*/

#endif
