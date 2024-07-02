#include "dutils.h"

#ifndef ARDUINO
    #include <chrono>
    #include <thread>

    /**
     * @brief Stops thread for milliseconds.
     * 
     * @param ms milliseconds to pause for.
     */
    void delay(unsigned long ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    /**
     * @brief Stops thread for microsecondss.
     * 
     * @param ms milliseconds to pause for.
     */
    void delayMicroseconds(unsigned long us) {
        std::this_thread::sleep_for(std::chrono::microseconds(us));
    }

    /**
     * @brief Get current timestamp in milliseconds (c++ equivalent of arduino millis()).
     * 
     * @return unsigned long milliseconds value.
     */
    unsigned long millis(void) {
        return(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    }
    /*
    // Posix version
    long millis(){
        struct timespec _t;
        clock_gettime(CLOCK_REALTIME, &_t);
        return _t.tv_sec*1000 + lround(_t.tv_nsec/1.0e6);
    }
    */
   
    /**
     * @brief Get current timestamp in microseconds (c++ equivalent of arduino micros()).
     * 
     * @return unsigned long microseconds value.
     */
    unsigned long micros(void) {
        // c++ equivalent of arduino micros()
        return(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    }

    // TODO: #define mapValue(x,in_min,in_max,out_min,out_max) (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min
#endif