# ddigitabutton

Class for easy use a pin with a push button.

## Features:

- Polling mode: periodically call read() and manually check result for state.

- Callback mode: set a callback for state changes and periodically only call read().

- Can adjust trigger ms for  pressed, long pressed, and double pressed.

- 8 states total handled:
  
  - RELEASE
  
  - PRESS
  
  - PRESSED
  
  - LONG_PRESSED
  
  - LONG_PRESSING
  
  - DBL_PRESSED
  
  - DBL_PRESSING
  
  - RELEASED

As Arduino style, you need to call begin() after instantiate the class.

See [example](examples/ddigitalbutton) for how to use.
