#include "utils.h"

#ifndef ARDUINO
    #include <chrono>
    #include <thread>

    /**
     * @brief Stops thread for milliseconds.
     * 
     * @param ms milliseconds to pause for.
     */
    void delay(unsigned long ms) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
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
        // c++ equivalent of arduino millis()
        return(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    }
#endif