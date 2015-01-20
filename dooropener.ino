#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>


#define neopixel_pin 6  // the digital pin the neopixel ring is connected to
#define orange strip.Color(139, 69, 0)
#define all_off strip.Color(0, 0, 0)
#define lockswitch 4 // the digital pin with the microswitch connected to the lock

int timer_before_closing_duration = 2000; // 2 seconds * 24 pixels = 48 secs before closing

Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, neopixel_pin, NEO_GRB + NEO_KHZ800);

Bounce lockswitchdebouncer = Bounce();



void setup() {

  //Set up the lock switch and his debouncer
  pinMode(lockswitch, INPUT);
  digitalWrite(lockswitch, HIGH);
  lockswitchdebouncer.attach(lockswitch);
  lockswitchdebouncer.interval(50);

  strip.begin();
  strip.show();
  timer_before_closing();

}

void loop() {
  lockswitchdebouncer.update();

  if (lockswitchdebouncer.read() == LOW){
    colorWipe(orange, 100); // The lock is armed
  }else{
    colorWipe(all_off, 100); // The lock is NOT armed
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
