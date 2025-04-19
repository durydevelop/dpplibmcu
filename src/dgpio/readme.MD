# dgpio module

This is the core module of the library. On SBC (like Raspberry pi) it is built on top of awesome [lg]([GitHub - joan2937/lg: Linux C libraries and Python modules for manipulating GPIO](https://github.com/joan2937/lg)) library by [joan2937]([joan2937 (joan2937) · GitHub](https://github.com/joan2937)) (the author of famous [pigpio]([GitHub - joan2937/pigpio: pigpio is a C library for the Raspberry which allows control of the General Purpose Input Outputs (GPIO).](https://github.com/joan2937/pigpio)) lib).

The new lg library use the linux kernel driver that handle /dev/gpiochip fd through ioctl api, so can work on any SBC with a gpio chipset on board.

## dgpiochip.cpp dgpiochip.h

Class for initializethe chip device and hold the handle for for using with gpio functions on SBC. On Arduino framework does nothing

## gpio.cpp gpio.h

Is not a class but just a wrapper api for base manipulation: setting pin mode, writing and reading pin.

Usually it is not necessary use them directly, because are used by high level class in this lib. You may need to use them if you want to add your own library module.

## derrorcodes.h

Contains error codes and error handling api.

## Arduino notes
To use unified source code in promrams, some methods need by lg library are available also in Arduino enviroment but are simply dummy methods that returns always a positive result.