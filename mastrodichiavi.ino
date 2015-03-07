#include <Stepper.h>
#include <Adafruit_NeoPixel.h>

#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty
#define neopixel_pin 10  // the digital pin the neopixel ring is connected to

//Defining some colors
#define all_off strip.Color(0, 0, 0)
#define low_red strip.Color(3, 0, 0)
#define med_red strip.Color(20, 0, 0)
#define hi_red strip.Color(40, 0, 0)
#define low_green strip.Color(0, 3, 0)
#define med_green strip.Color(0, 20, 0)
#define hi_green strip.Color(0, 40,0)
#define orange strip.Color(139, 69, 0)

int timer_before_closing_duration = 1000; // 2 seconds * 24 pixels = 48 secs before closing


// here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
byte buttons[] = {4, 5, 7};

// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)

// we will track if a button is just pressed, just released, or 'currently pressed'
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];


//Defining the motor shield pins
const int pwmA = 3;
const int brakeB = 8;
const int brakeA = 9;
const int pwmB = 11;
const int dirA = 12;
const int dirB = 13;

// Steps 200 is a complete round
const int STEPS = 416;

// Initialize the Stepper
Stepper stepperMotor(STEPS, dirA, dirB);

// Initialize the LedRing
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, neopixel_pin, NEO_GRB + NEO_KHZ800);

void setup() {
byte i;

// set up serial port
Serial.begin(9600);

// Make input & enable pull-up resistors on switch pins
for (i=0; i< NUMBUTTONS; i++) {
  pinMode(buttons[i], INPUT);
  digitalWrite(buttons[i], HIGH);
}

// send an intro:
Serial.println("\n\n Mastro di chiavi [Booting...]\n");
Serial.println();

//Initialize the led ring
strip.begin();
strip.show();

// Initialize the stepper motor
pinMode(pwmA, OUTPUT);
pinMode(pwmB, OUTPUT);
pinMode(brakeA, OUTPUT);
pinMode(brakeB, OUTPUT);

// Set the stepper rotation speed a good value found with a potentiometer is ~ 65/75 rpm
stepperMotor.setSpeed(65);

//Booting routine
theaterChase(strip.Color(  0,   0, 127), 50, 25); // Blue
color_wipe(strip.Color(0, 0, 0), 5);    // Black/off

// send an intro:
Serial.println("\nMastro di chiavi [Ready]\n");
Serial.println();

if(lockclosed() == false){
  fade_up(200, 10, 0, 40, 0); //hi green
}else{
  fade_up(200, 10, 40, 0, 0); //hi red
}

}


void loop() {
check_switches();
do_actions();
}

void check_switches()
{
static byte previousstate[NUMBUTTONS];
static byte currentstate[NUMBUTTONS];
static long lasttime;
byte index;
if (millis() < lasttime) {
   lasttime = millis(); // we wrapped around, lets just try again
}

if ((lasttime + DEBOUNCE) > millis()) {
  return; // not enough time has passed to debounce
}
// ok we have waited DEBOUNCE milliseconds, lets reset the timer
lasttime = millis();

for (index = 0; index < NUMBUTTONS; index++) {
  justpressed[index] = 0;       // when we start, we clear out the "just" indicators
  justreleased[index] = 0;

  currentstate[index] = digitalRead(buttons[index]);   // read the button
  if (currentstate[index] == previousstate[index]) {
    if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
    }
    else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        // just released
        justreleased[index] = 1;
    }
    pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
  }
  //Serial.println(pressed[index], DEC);
  previousstate[index] = currentstate[index];   // keep a running tally of the buttons
}
}

void do_actions(){
if (justpressed[1]){
  Serial.println("Segnale di apertura (NFC  Pulsante)");
  if( lockclosed() == true ){
    Serial.println("La porta e' chiusa, la devo aprire...");
    turn_key("open");
    //effects_after_opening();
  }else{
    Serial.println("La porta e' gia' aperta, non devo fare niente!");
    theaterChase(random_color(),50,15);
    fade_up(200, 10, 0, 40, 0); //hi green
  }
}

if (justpressed[2]){
  Serial.println("Segnale di chiusura ricevuto...");
  if ( lockclosed() == false ){
    Serial.println("La porta e' aperta, la devo chiudere...");
    delay(15000);
    //fade_up(100, 20, 238, 238, 0); //yellow
    //color_wipe(orange, timer_before_closing_duration); // orange
    turn_key("close");
    Serial.println("Door closed");

  }else{
    Serial.println("La porta e' gia' chiusa, non devo fare niente.");
    theaterChase(random_color(),50,15);
    fade_up(200, 10, 40, 0, 0); //hi red
  }
}

}

boolean lockclosed(){
 check_switches();
 if(pressed[0]){
   Serial.println("Paletto aperto!");
   return false;
 }else{
   Serial.println("Paletto chiuso!");
   return true;
 }
}

void turn_key(String direction){
 uint16_t i,j;
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
     digitalWrite(pwmA, HIGH);
     digitalWrite(pwmB, HIGH);
     digitalWrite(brakeA, LOW);
     digitalWrite(brakeB, LOW);
     if(direction == "close"){
         stepperMotor.step((STEPS / strip.numPixels()));
       }else{
         stepperMotor.step(-(STEPS / strip.numPixels()));
     }
     digitalWrite(pwmA, LOW);
     digitalWrite(pwmB, LOW);
     digitalWrite(brakeA, HIGH);
     digitalWrite(brakeB, HIGH);
     strip.show();
   }

 }

 if(direction == "close"){
     fade_up(200, 10, 40, 0, 0); //hi red
   }else{
     fade_up(200, 10, 0, 40, 0); //hi green
 }

}

// Function to get some random color, to be used with strip.setPixelColor(i, random_color());
uint32_t random_color(){
 long R,B,G;
 uint32_t rnd_color = strip.Color(random(0,255), random(0,255), random(0,255));
 return rnd_color;
}

// Function to get some fancy led effects after opening the door
void effects_after_opening(){
 randomSeed(analogRead(0));
 uint32_t trick = random(1,5);
 switch(trick){
   case 1:
     theaterChaseRainbow(50); //A 50ms delay corresponds to ~40 sec loop
   break;

   case 2:
     theaterChase(random_color(),50,250); //A 50ms delay with 250 cycles corresponds to ~40 sec loop
   break;

   case 3:
     rainbow(150); //A 150ms delay corresponds to ~40 sec loop
   break;

   case 4:
     rainbowCycle(50); //A 50ms delay corresponds to ~40 sec loop
   break;
 }
}


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
}

// Fill the dots one after the other with a color
void color_wipe(uint32_t c, int wait) {
 for(uint16_t i=0; i<strip.numPixels(); i++) {
     strip.setPixelColor(i, c);
     strip.show();
     delay(wait);
 }
}

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

 for(j=0; j<256*3; j++) { // 3 cycles of all colors on wheel
   for(i=0; i< strip.numPixels(); i++) {
     strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
   }
   strip.show();
   delay(wait);
 }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait, int numcycles) {
 for (int j=0; j < numcycles; j++) {  //do numcycles cycles of chasing
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
