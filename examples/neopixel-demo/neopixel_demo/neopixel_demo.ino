/* 
 * Modular NeoPixel Control Board
 * 
 * Control upto 4 strips of NeoPixels individually. 
 * Board features include speed, brightness, and mode selection
 * TO DO: Create interface with Amazon Alexa via wifi network
 * 
 * Developed and built on Adafruit_NeoPixel.h library
 * https://github.com/adafruit/Adafruit_NeoPixel
 * 
 * Copyright (c) 2021 Will Donaldson
 * MIT License (MIT)
 * www.neopixel-controller.willdonaldson.io
 */

#include <Adafruit_NeoPixel.h>

#define LEDpin_1 D7
#define LEDpin_2 D6
#define LEDpin_3 D4
#define LEDpin_4 D3

#define analogPin A0    
#define pot1_enable D1    // speed potentiometer
#define pot2_enable D2    // brightness potentiometer
#define modePin D5

bool enable_input = true; // choose to enable or disable potentiometer and momentary button input unser controls
int Brightness = 150;     // range of [0, 255]
int Speed = 50;           // range of [0, 100]
int Mode = 1;             // choose lighting mode

static unsigned long t_Prev = 0;
int debounce_delay = 350;
void ICACHE_RAM_ATTR buttonISR();
bool breakLoop = false;

// 4 strips can be uniquely controlled. But for demo script only one is used
#define LEDcount_1 25
#define LEDcount_2 0
#define LEDcount_3 0
#define LEDcount_4 0

Adafruit_NeoPixel strip1(LEDcount_1, LEDpin_1, NEO_GRB + NEO_KHZ800); //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
Adafruit_NeoPixel strip2(LEDcount_2, LEDpin_2, NEO_GRB + NEO_KHZ800); //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
Adafruit_NeoPixel strip3(LEDcount_3, LEDpin_3, NEO_GRB + NEO_KHZ800); //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
Adafruit_NeoPixel strip4(LEDcount_4, LEDpin_4, NEO_GRB + NEO_KHZ800); //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

uint32_t red = strip1.Color(255, 0, 0);
uint32_t green = strip1.Color(0, 255, 0);
uint32_t blue = strip1.Color(0, 0, 255);
uint32_t white = strip1.Color(255, 255, 255);
uint32_t off = strip1.Color(0, 0, 0);
uint32_t cyan = strip1.Color(0, 255, 255);
uint32_t magenta = strip1.Color(255, 0, 51);
uint32_t pink = strip1.Color(255, 51, 119);
uint32_t aqua = strip1.Color(85, 125, 255);
uint32_t violet = strip1.Color(153, 0, 255);
uint32_t yellow = strip1.Color(255, 170, 0);

uint32_t colour_array[] = {red, green, blue, white, cyan, magenta, pink, aqua, violet, yellow};
int number_of_colours =10;

void setup() {
  strip1.begin();           // initialize neopixel strip 
  strip1.show();  
  if(enable_input == true){
    pinMode(analogPin, INPUT);
    pinMode(pot1_enable, OUTPUT);
    pinMode(pot2_enable, OUTPUT);
    digitalWrite(pot1_enable, LOW);         // disengage both potentiometers
    digitalWrite(pot2_enable, LOW); 
    pinMode(modePin, INPUT);   // uses external pullup resistor
    attachInterrupt(digitalPinToInterrupt(modePin), buttonISR, FALLING);
  }
}

void loop() {
  if(Mode == 1){
    colorWipe(); 
  }
  
  else if(Mode == 2){
    theaterChase(); 
  }
  
  else if(Mode == 3){
    rainbow();             
  }
  
  else if(Mode == 4){
    theaterChaseRainbow(); 
  }
}

int read_pot1(){
  digitalWrite(pot1_enable, HIGH);        // temporarily engage one potentiometer on
  int val = analogRead(analogPin);
  digitalWrite(pot1_enable, LOW);         // disengage potentiometer off again
  return rescale_analog(val, 0, 100);
}

int read_pot2(){
  digitalWrite(pot2_enable, HIGH); 
  int val = analogRead(analogPin);
  digitalWrite(pot2_enable, LOW);
  return rescale_analog(val, 0, 255);
}

int rescale_analog(int val, int Min, int Max){
  // IN4001 has a voltage drop of ~0.7V
  // very hacky impementation to rescale range of input analog values. Doesn't accunt for voltage drop
  // TO DO: revisit and rewrite. Or just replace diodes with analog multiplexer
  int rescaled_val = constrain(map(val, 10, 890, Min, Max), Min, Max);  
  return rescaled_val;
}

void checkUserInput(){
  if(enable_input == true){
    Speed = read_pot1();
    Brightness = read_pot2();
    strip1.setBrightness(Brightness);
  } 
}

void buttonISR(){
  breakLoop = true; // after ISR finishes, break out of current animation and move to next animation
  unsigned long t_Now = millis();
  if(t_Now - t_Prev > debounce_delay){
    Mode += 1;
  }
  if(Mode > 4){       // adjust number based on number of animations
    Mode = 1;         // loop back to first animation
  }
  t_Prev = t_Now;
}

void colorWipe() {
  for(int j=0; j<number_of_colours; j++){
    for(int i=0; i<strip1.numPixels(); i++) {
      if(breakLoop == true){        // exit animation if button is pressed
        breakLoop = false;
        return;
      }
      checkUserInput();
      strip1.setPixelColor(i, colour_array[j]);       
      strip1.show();                       
      delay(100 - Speed);     
    }
  }
}

void theaterChase() {
  for(int j=0; j<number_of_colours; j++){
    for(int a=0; a<10; a++) {  // Repeat 10 times...
      for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
        if(breakLoop == true){        // exit animation if button is pressed
          breakLoop = false;
          return;
        }
        checkUserInput();
        strip1.clear();         //   Set all pixels in RAM to 0 (off)
        // 'c' counts up from 'b' to end of strip in steps of 3...
        for(int c=b; c<strip1.numPixels(); c += 3) {
          strip1.setPixelColor(c, colour_array[j]); // Set pixel 'c' to value 'color'
        }
        strip1.show(); 
        delay(100 - Speed); 
      }
    }
  }
}

void rainbow() {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip1.numPixels(); i++) { // For each pixel in strip...   
      if(breakLoop == true){        // exit animation if button is pressed
        breakLoop = false;
        return;
      }
      checkUserInput();
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip1.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip1.setPixelColor(i, strip1.gamma32(strip1.ColorHSV(pixelHue)));
    }
    strip1.show();
    delay(map(100 - Speed, 0, 100, 0, 15));  // map function since this animation is significantly slower than the others without it
  }
}

void theaterChaseRainbow() {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      if(breakLoop == true){        // exit animation if button is pressed
        breakLoop = false;
        return;
      }
      checkUserInput();
      strip1.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip1.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip1.numPixels();
        uint32_t color = strip1.gamma32(strip1.ColorHSV(hue)); // hue -> RGB
        strip1.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip1.show();                
      delay(100 - Speed);
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
