# dpplib-mcu

dpplib-mcu is a library that allow you (...or aim to) writing program for MCUs (like Arduino) and SBCs (like Raspberry pi) using the same code.

For example, You can read a digital pin 4 instantiating the class and call begin() (arduino style):

```cpp
DDigitalInput input(4);
input.begin();
```

then read it:

```cpp
int status = input;
```

or:

```cpp
int status=input.read();
```

The result can be HIGH or LOW (Arduino style).

This code compiles on both Arduino and Raspberry.

***

#### See [here](src) for modules list and help.

### Doxygen support for doc.

### Module drivers ready to use for sensor board:

- INA226 and INA228 current/voltage sensors.
