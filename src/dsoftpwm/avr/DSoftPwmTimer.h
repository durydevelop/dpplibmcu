#define TIMER_FREQ 50000

/**
 * TODO:
 *      _ TIMER_TO_USE_FOR_MILLIS
 *      _ MILLIS_TIMER
 *      _ __AVR_ATtiny85__
 */

// AVR Timer CTC Interrupts Calculator
// v. 8
// http://www.arduinoslovakia.eu/application/timer-calculator

void SetupTimer(void)
{
    noInterrupts();
        #if defined(__AVR_ATtiny85__)
            // Microcontroller: ATtiny85 Timer0
            // Clear registers
            TCCR0A = 0;
            TCCR0B = 0;
            TCNT0 = 0;

            // 50000 Hz (16000000/((4+1)*64))
            OCR0A = 4;
            // CTC
            TCCR0A |= (1 << WGM01);
            // Prescaler 64
            TCCR0B |= (1 << CS01) | (1 << CS00);
            // Output Compare Match A Interrupt Enable
            TIMSK |= (1 << OCIE0A);
        #elif defined(__AVR_ATmega328P) || defined(__AVR_ATmega328)
            // Microcontroller: ATmega328P Timer1
            // Clear registers
            TCCR1A = 0;
            TCCR1B = 0;
            TCNT1 = 0;

            // 50000 Hz (16000000/((4+1)*64))
            OCR1A = 4;
            // CTC
            TCCR1B |= (1 << WGM12);
            // Prescaler 64
            TCCR1B |= (1 << CS11) | (1 << CS10);
            // Output Compare Match A Interrupt Enable
            TIMSK1 |= (1 << OCIE1A);
        #endif
    interrupts();
}