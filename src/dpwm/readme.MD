# dpwm

Class for easy use a pin with as PWM output.

On both SBC and MCU, pwm signal is software timed, so **any pin can be used** as pwm output.

## Features:

- Direct setting frequence.

- Set duty cycle percentual or by microseconds.

- Block output in HIGH or LOW level.

As Arduino style, you need to call begin() after instantiate the class.

See [example](examples/sbc-pwm-demo) for how to use.

## Limits:

- On Arduino framework, only ARDUINO_ARCH_AVR platform is supported.
