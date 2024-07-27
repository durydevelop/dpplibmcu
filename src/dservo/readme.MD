# dservo

Class for using servo motors with a lot of usefull functions.

dservo uses [dpwm](src/dpwm) lib generate pulses for running servo.

## Features:

- All api from arduino servo library are wrapped, so, you can use same code.

- set position in range of  [0.0 - 1.0] where 0.05 is the center position

- Set an arbitrary custom home position that can be reached calling home().

- Can adjust full clockwise and counter-clockwise end positions.

- Store position.

- Set and get positon in degrees or microseconds.

As Arduino style, you need to call begin() after instantiate the class.

See [example](examples/dservo) for how to use.

## Limits:

- Same of [dpwm](src/dpwm).
