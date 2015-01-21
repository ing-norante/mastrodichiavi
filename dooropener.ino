#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>
#include <Stepper.h>


//Arduino PIN definition
#define neopixel_pin 6  // the digital pin the neopixel ring is connected to
#define lockswitch 4 // the digital pin with the microswitch connected to the lock
#define nfcswitch 5 // the digital pin where arrives the signal from the Came RBM21


//Defining some colors
#define all_off strip.Color(0, 0, 0)
#define low_red strip.Color(3, 0, 0)
#define med_red strip.Color(20, 0, 0)
#define hi_red strip.Color(40, 0, 0)
#define low_green strip.Color(0, 3, 0)
#define med_green strip.Color(0, 20, 0)
#define hi_green strip.Color(0, 40,0)
#define orange strip.Color(139, 69, 0)


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

//Initialize the NeoPixel Led Ring
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, neopixel_pin, NEO_GRB + NEO_KHZ800);

// Instantiate some bounce objects
Bounce lockswitchdebouncer = Bounce();


bool oldState = HIGH;


void setup() {

  //Set up the lock switch and his debouncer
  pinMode(lockswitch, INPUT);
  digitalWrite(lockswitch, HIGH);
  lockswitchdebouncer.attach(lockswitch);
  lockswitchdebouncer.interval(50);

  //Set up the switch from the came rbm21
  pinMode(nfcswitch, INPUT_PULLUP);


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
      turn_key("close");
  }

  randomSeed(analogRead(0));
}

void loop() {
  // Get current button state.
  bool newState = digitalRead(nfcswitch);

  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(nfcswitch);
    if (newState == LOW) {
      turn_key("open");
    }
  }

  // Set the last button state to the old state.
  oldState = newState;
}


void turn_key(String direction){

  uint16_t i,j;

  //Two turns...
  for( j=0; j<2; j++){
    for (i=0; i< strip.numPixels();i++){
      switch(j){
        case 0:
          if(direction == "close"){
            strip.setPixelColor(i, low_red);
          }else{
            strip.setPixelColor(i, low_green);
          }

        break;

        case 1:
          if(direction == "close"){
            strip.setPixelColor(i, med_red);
            }else{
              strip.setPixelColor(i, med_green);
          }
        break;

      }
      if(direction == "close"){
          stepperMotor.step(-(STEPS / strip.numPixels()));
        }else{
          stepperMotor.step(STEPS / strip.numPixels());
      }

      strip.show();
    }
  }

  //...and an half
  for (int k=0; k < TURN_FIXTURE; k++){
    if(direction == "close"){
      stepperMotor.step(-1);
    }else{
        stepperMotor.step(1);
    }

  }

  if(direction == "close"){
    fade_up(200, 10, 40, 0, 0); //hi red
    }else{
      stepperMotor.step(-1);
  }

}//turn_key


// Function to get some random color, to be used with strip.setPixelColor(i, random_color());
uint32_t random_color(){
  long R,B,G;
  uint32_t rnd_color = strip.Color(random(0,255), random(0,255), random(0,255));
  return rnd_color;
}

// This is the function that is fired before the lock gets closed, for some fancy effects on the ledring
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

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else if(WheelPos < 170) {
      WheelPos -= 85;
      return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
      } else {
        WheelPos -= 170;
        return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
      }
    }
