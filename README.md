#Arduino Door Opener for [OfficineNora](http://www.officinenora.it)#
This system is designed to open *automatically* the door at Officine Nora Jewlerly workshop in Florence using this hardware:
* Arduino Uno
* Stepper Motor
* Came RMB21
* Neopixel Led Ring

When I say *automatically* I mean using the stepper motor to phisically turn the key inside the keylock. The access control is ruled by
the Came RBM21 with an RFID reader sensor placed near the entrance.

###Basic functioning###
The Arduino has a power switch and a switch sensor placed inside the keylock to detect if the lock is armed or not. The power switch is used
to start the Arduino, if the power switch is on off the Arduino will not be working and the door will be opened manually or via the RBM21.
When you start the Arduino it will:

* Check the state of the switch sensor:
 * Open => Wait 48 seconds and then fire the Closing routine.
 * Closed => Somebody had already closed the door mechanically. Do nothing.
