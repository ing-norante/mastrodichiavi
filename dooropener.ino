#include <Adafruit_NeoPixel.h>

#define PIN 6  // the digital pin the data line is connected to
#define hi_gold strip.Color(255, 215, 0) 
#define goldenrod strip.Color(218,165,32)
#define all_off strip.Color(0, 0, 0)
int timer_before_closing_duration = 2000; // 2 seconds * 24 pixels = 48 secs before closing

Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);



void setup() {
  strip.begin();
  strip.show(); 
  timer_before_closing();
}

void loop() {
} 


void timer_before_closing(){
  fade_up(100, 30, 255, 69, 0); //golden rod(255,69,0)
  colorWipe(hi_gold, timer_before_closing_duration); // Gold
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



