#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>
#include <Stepper.h>



#define neopixel_pin 6  // the digital pin the neopixel ring is connected to
#define orange strip.Color(139, 69, 0)
#define all_off strip.Color(0, 0, 0)
#define low_red strip.Color(3, 0, 0)
#define low_green strip.Color(0, 3, 0)
#define med_red strip.Color(20, 0, 0)
#define med_green strip.Color(0, 20, 0)
#define hi_red strip.Color(40, 0, 0)
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
const int STEPS = 208;
const int TURN_FIXTURE = 108;

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
      //stepperMotor.step(STEPS);
      close_lock();
  }

  randomSeed(analogRead(0));
}

void loop() {

  // Update the nfcdebouncer
  nfcdebouncer.update();

  // Open or close the lock with the stepper motor
  if ( nfcdebouncer.read() == LOW) {
    close_lock();
  }
  else {

  }
}


void close_lock(){

  uint16_t i,j;

  //Two turns
  for( j=0; j<2; j++){
    for (i=0; i< strip.numPixels();i++){
      strip.setPixelColor(i, random_color());
      strip.show();
      stepperMotor.step(STEPS / strip.numPixels());
    }
  }

  //Some fixtures
  for (int k=0; k < TURN_FIXTURE; k++){
    stepperMotor.step(1);
  }

  fade_up(100, 20, 40, 0, 0); //hi red

  /*for (j=0; j<2; j++) {
    for(i=0; i<strip.numPixels()+3; i++) {
      if (i<strip.numPixels()) strip.setPixelColor(i, hi_red);
      if ((i-1 >= 0) && (i-1 < strip.numPixels())) strip.setPixelColor(i-1, med_red);
      if ((i-2 >= 0) && (i-2 < strip.numPixels())) strip.setPixelColor(i-2, low_red);
      if ((i-3 >= 0) && (i-3 < strip.numPixels())) strip.setPixelColor(i-3, med_green);
      strip.show();
      delay(50);
    }
  }*/
}//close_lock


// Function to get some random color, to be used with strip.setPixelColor(i, random_color());
uint32_t random_color(){
  long R,B,G;
  uint32_t rnd_color = strip.Color(random(0,255), random(0,255), random(0,255));
  return rnd_color;
}


void timer_before_closing(){
  fade_up(100, 20, 238, 238, 0); //yellow
  color_wipe(orange, timer_before_closing_duration); // orange
  //color_wipe(all_off, 100); // Off
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
void color_wipe(uint32_t c, int wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
} //color_wipe
