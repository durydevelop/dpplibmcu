#ifndef DUtils_H
#define DUtils_H

#ifndef ARDUINO
    void delay(unsigned long ms);
    void delayMicroseconds(unsigned long us);
    unsigned long millis(void);
    unsigned long micros(void);

    //#define mapValue(x,in_min,in_max,out_min,out_max) (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min
    //template <typename T>
    //T mapValue(T x, T in_min, T in_max, T out_min, T out_max) { return(x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min; }
#endif

#endif
