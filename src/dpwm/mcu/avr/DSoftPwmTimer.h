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
            // ATtiny85 Timer1 (modalità normale, overflow)
            #define TIMER_FREQ 244 // Max freq
            TCCR1 = 0;
            GTCCR |= (1 << PSR1);  // Reset prescaler
            TCNT1 = 0;
            TCCR1 |= (1 << CS13);  // Prescaler 256 → Overflow ~244 Hz (16MHz / 256 / 256)
            TIMSK |= (1 << TOIE1); // Interrupt su overflow
        #endif
    #elif defined (__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
        #ifdef DPWM_USE_TIMER2
            // Microcontroller: ATmega328P Timer2
            // Clear registers
            TCCR2A = 0;
            TCCR2B = 0;
            TCNT2 = 0;

            #if (TIMER_FREQ == 31250)
              // 31250 Hz (16000000/((1+1)*256))
              OCR2A = 1;
              // CTC
              TCCR2A |= (1 << WGM21);
              // Prescaler 256
              TCCR2B |= (1 << CS22) | (1 << CS21);
            #elif (TIMER_FREQ == 32000)
              // 32000 Hz (16000000/((499+1)*1))
              OCR2A = 499;
              // CTC
              TCCR2A |= (1 << WGM21);
              // Prescaler 1
              TCCR2B |= (1 << CS20);
            #elif (TIMER_FREQ == 40000)
              // 40000 Hz (16000000/((49+1)*8))
              OCR2A = 49;
              // CTC
              TCCR2A |= (1 << WGM21);
              // Prescaler 8
              TCCR2B |= (1 << CS21);
            #elif (TIMER_FREQ == 25000)
              // 25000 Hz (16000000/((9+1)*64))
              OCR2A = 9;
              // CTC
              TCCR2A |= (1 << WGM21);
              // Prescaler 64
              TCCR2B |= (1 << CS22);
            #elif (TIMER_FREQ == 50000)
              // 50000 Hz (16000000/((4+1)*64))
              OCR2A = 4;
              // CTC
              TCCR2A |= (1 << WGM21);
              // Prescaler 64
              TCCR2B |= (1 << CS22);
            #endif

            // Output Compare Match A Interrupt Enable
            TIMSK2 |= (1 << OCIE2A);
        #else
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
    #endif
    interrupts();
    US_PER_TICK=1000000/TIMER_FREQ; // same as (1/TIMER_FREQ)*1000000 using int instead of float
    return true;
  #else
    return false;
  #endif
}