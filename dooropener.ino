#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>
#include <Stepper.h>



#define neopixel_pin 6  // the digital pin the neopixel ring is connected to
#define orange strip.Color(139, 69, 0)
#define all_off strip.Color(0, 0, 0)
#define lockswitch 4 // the digital pin with the microswitch connected to the lock
#define nfcswitch 5 // the digital pin where arrives the signal from the Came RBM21

int timer_before_closing_duration = 100; // 2 seconds * 24 pixels = 48 secs before closing

//Defining the motor shield pins
const int pwmA = 3;
const int pwmB = 11;
const int brakeA = 9;
const int brakeB = 8;
const int dirA = 12;
const int dirB = 13;

// Steps 200 is a complete round
const int STEPS = 200;

// Initialize the Stepper
Stepper stepperMotor(STEPS, dirA, dirB);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, neopixel_pin, NEO_GRB + NEO_KHZ800);

// Instantiate some bounce objects
Bounce lockswitchdebouncer = Bounce();
Bounce nfcdebouncer = Bounce();



void setup() {

  //Set up the lock switch and his debouncer
  pinMode(lockswitch, INPUT);
  digitalWrite(lockswitch, HIGH);
  lockswitchdebouncer.attach(lockswitch);
  lockswitchdebouncer.interval(50);

  //Set up the switch from the came rbm21 and his debouncer
  pinMode(nfcswitch,INPUT);
  digitalWrite(nfcswitch,HIGH);
  nfcdebouncer.attach(nfcswitch);
  nfcdebouncer.interval(50);


  //Initialize the led ring
  strip.begin();
  strip.show();

  // Turn on pulse width modulation
  pinMode(pwmA, OUTPUT);
  digitalWrite(pwmA, HIGH);
  pinMode(pwmB, OUTPUT);
  digitalWrite(pwmB, HIGH);

  // Unleash the breaks
  pinMode(brakeA, OUTPUT);
  digitalWrite(brakeA, LOW);
  pinMode(brakeB, OUTPUT);
  digitalWrite(brakeB, LOW);

  // Set the stepper rotation speed a good value found with a potentiometer is ~ 65/75 rpm
  stepperMotor.setSpeed(65);

  lockswitchdebouncer.update();

  if (lockswitchdebouncer.read() == LOW){
    // The lock is armed
    }else{
      // The lock is NOT armed
      timer_before_closing();
      stepperMotor.step(STEPS);
    }

}

void loop() {

  // Update the nfcdebouncer
  nfcdebouncer.update();

  // Open or close the lock with the stepper motor
  if ( nfcdebouncer.read() == LOW) {
    stepperMotor.step(-STEPS); //I used -STEPS to turn in the opposite direction
  }
  else {

  }
}


void timer_before_closing(){
  fade_up(100, 20, 238, 238, 0); //yellow
  colorWipe(orange, timer_before_closing_duration); // orange
  colorWipe(all_off, 100); // Off
}//timer_before_closing


// fade_up - fade up to the given color
void fade_up(int num_steps, int wait, int R, int G, int B) {
   uint16_t i, j;

   for (i=0; i<num_steps; i++) {
      for(j=0; j<strip.numPixels(); j++) {
         strip.setPixelColor(j, strip.Color(R * i / num_steps, G * i / num_steps, B * i / num_steps));
      }
   strip.show();
   delay(wait);
   }
} // fade_up



// Fill the dots one after the other with a color
void colorWipe(uint32_t c, int wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
} //colorWipe
