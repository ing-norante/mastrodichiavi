#Arduino Door Opener for [OfficineNora](http://www.officinenora.it)#
This system is designed to open *automatically* the door at Officine Nora Jewlerly workshop in Florence using this hardware:
* [Arduino Uno r3](http://arduino.cc/en/Main/ArduinoBoardUno)  
* [Kysan 1124090 Nema 17 Stepper Motor](http://store.arduino.cc/product/MK00742)
* [Arduino Motor Shield Rev3](http://store.arduino.cc/product/A000079)
* [Came RMB21 Access control unit](http://www.cameuk.com/files/pdf/rbm21/RBM21_EN.pdf)
* [NeoPixel Ring - 24 x WS2812 5050 RGB LED with Integrated Drivers](http://www.adafruit.com/product/1586)

When I say *automatically* I mean using the stepper motor to phisically turn the key inside the keylock. The access control is ruled by
the Came RBM21 with an RFID reader sensor placed near the entrance.

###Basic functioning###
The Arduino has a power switch and a switch sensor placed inside the keylock to detect if the lock is armed or not. The power switch is used
to start the Arduino, if the power switch is *off* the Arduino will not be working and the door will be opened manually or via the RBM21.
When you start the Arduino it will:

* Check the state of the switch sensor:
 * Open => Wait ~40 seconds and then fire the Closing routine.
 * Closed => Somebody had already closed the door mechanically. Do nothing.

####Closing Routine####
This is a function needed to let the Arduino wait ~40 seconds before closing the door.
This is because we have a security system that has the same timer before getting armed. During this time it's funny to show some tricks on the
led ring that's why timer_before_closing() has some random scenario to amuse you during the wait.  



###Libraries Used###
I tried to use only *official* libraries when available:

* [Bounce](http://playground.arduino.cc/Code/Bounce) - Arduino library that debounces digital inputs
* [Adafruit NeoPixel library](https://github.com/adafruit/Adafruit_NeoPixel) - Arduino library for controlling single-wire-based LED pixels and strip
* [SMLib](http://playground.arduino.cc/Code/SMlib) - A simple library for finite state machines
* [Stepper](http://arduino.cc/en/reference/stepper) - A stepper library to control control unipolar or bipolar stepper motors
