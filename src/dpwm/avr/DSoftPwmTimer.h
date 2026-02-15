#include <Arduino.h>

/**
 * @brief Timer functions for AVR cpu.
  
  For now only:
  - ATtiny85:
    default Timer0
  - ATmega328P, ATmega328:
    default Timer1
  
 */

/** Possible value for timer frequences
 * al least one must be uncommented
 * to make timer work
*/
//#define TIMER_FREQ 50000
//#define TIMER_FREQ 25000
//#define TIMER_FREQ 40000
//#define TIMER_FREQ 32000
#define TIMER_FREQ 31250

static uint16_t US_PER_TICK=0;

/**
 * TODO:
 *      - Check defines:
 *                      - ATtiny: TIMER_TO_USE_FOR_MILLIS
 *                      - ATmega: MILLIS_TIMER
 */

// AVR Timer CTC Interrupts Calculator
// v. 8
// http://www.arduinoslovakia.eu/application/timer-calculator

bool SetupTimer(void)
{
  #ifdef TIMER_FREQ
    noInterrupts();
    #if defined(__AVR_ATtiny85__)
        #ifndef DPWM_USE_TIMER1
            // Microcontroller: ATtiny85 Timer0 (default)
            // Clear registers
            TCCR0A = 0;
            TCCR0B = 0;
            TCNT0 = 0;

            #if (TIMER_FREQ == 50000)
            // 50000 Hz (16000000/((4+1)*64))
            OCR0A = 4;
            // CTC
            TCCR0A |= (1 << WGM01);
            // Prescaler 64
            TCCR0B |= (1 << CS01) | (1 << CS00);
            #elif (TIMER_FREQ == 31250)
            // 31250 Hz (16000000/((1+1)*256))
            OCR0A = 1;
            // CTC
            TCCR0A |= (1 << WGM01);
            // Prescaler 256
            TCCR0B |= (1 << CS02);
            #elif (TIMER_FREQ == 25000)
            // 25000 Hz (16000000/((9+1)*64))
            OCR0A = 9;
            // CTC
            TCCR0A |= (1 << WGM01);
            // Prescaler 64
            TCCR0B |= (1 << CS01) | (1 << CS00);
            #elif (TIMER_FREQ == 40000)
            // 40000 Hz (16000000/((49+1)*8))
            OCR0A = 49;
            // CTC
            TCCR0A |= (1 << WGM01);
            // Prescaler 8
            TCCR0B |= (1 << CS01);
            #endif

            // Output Compare Match A Interrupt Enable
            TIMSK |= (1 << OCIE0A);
        #else
            // Microcontroller: ATtiny85 Timer1
            // --- Stop Timer1 ---
            TCCR1 = 0;
            GTCCR = 0;      // Reset prescaler (importantissimo su tiny85)
            TCNT1 = 0;

            // --- CTC mode ---
            TCCR1 |= (1 << CTC1);

            #if (TIMER_FREQ == 50000)
                // 50000 Hz = 16MHz / (5 * 64)
                OCR1A = 4;
                TCCR1 |= (1 << CS12) | (1 << CS11);   // Prescaler 64

            #elif (TIMER_FREQ == 31250)
                // 31250 Hz = 16MHz / (2 * 256)
                OCR1A = 1;
                TCCR1 |= (1 << CS13);                 // Prescaler 256

            #elif (TIMER_FREQ == 25000)
                // 25000 Hz = 16MHz / (10 * 64)
                OCR1A = 9;
                TCCR1 |= (1 << CS12) | (1 << CS11);   // Prescaler 64

            #elif (TIMER_FREQ == 40000)
                // 40000 Hz = 16MHz / (50 * 8)
                OCR1A = 49;
                TCCR1 |= (1 << CS11);                 // Prescaler 8
            #endif

            // --- Enable Compare Match A interrupt ---
            TIMSK |= (1 << OCIE1A);
        #endif
    #elif defined (__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
        // Microcontroller: ATmega328P Timer1
        // Clear registers
        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1 = 0;

        #if (TIMER_FREQ == 31250)
          // 31250 Hz (16000000/((1+1)*256))
          OCR1A = 1;
          // CTC
          TCCR1B |= (1 << WGM12);
          // Prescaler 256
          TCCR1B |= (1 << CS12);
        #elif (TIMER_FREQ == 32000)
          // 32000 Hz (16000000/((499+1)*1))
          OCR1A = 499;
          // CTC
          TCCR1B |= (1 << WGM12);
          // Prescaler 1
          TCCR1B |= (1 << CS10);
        #elif (TIMER_FREQ == 40000)
          // 40000 Hz (16000000/((49+1)*8))
          OCR1A = 49;
          // CTC
          TCCR1B |= (1 << WGM12);
          // Prescaler 8
          TCCR1B |= (1 << CS11);
        #elif (TIMER_FREQ == 25000)
          // 25000 Hz (16000000/((9+1)*64))
          OCR1A = 9;
          // CTC
          TCCR1B |= (1 << WGM12);
          // Prescaler 64
          TCCR1B |= (1 << CS11) | (1 << CS10);
        #elif (TIMER_FREQ == 50000)
          // 50000 Hz (16000000/((4+1)*64))
          OCR1A = 4;
          // CTC
          TCCR1B |= (1 << WGM12);
          // Prescaler 64
          TCCR1B |= (1 << CS11) | (1 << CS10);
        #endif

        // Output Compare Match A Interrupt Enable
        TIMSK1 |= (1 << OCIE1A);
    #endif
    interrupts();
    US_PER_TICK=1000000/TIMER_FREQ; // same as (1/TIMER_FREQ)*1000000 using int instead of float
    return true;
  #else
    return false;
  #endif
}