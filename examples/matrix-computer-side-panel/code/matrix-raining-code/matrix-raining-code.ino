/* 
 * Code animates raining code (from The Matrix) on a custom computer side panel 
 * Computer side panel is modified with laser cut sheets of acrylic (see build video)
 * NeoPixel array is 18 x 12
 * Due to the winding nature of the electrical wiring every even column (first column is 0) must be animated in reverse order to every odd column.
 * 
 * Developed and built on Adafruit_NeoPixel.h library
 * https://github.com/adafruit/Adafruit_NeoPixel
 * 
 * Copyright (c) 2021 Will Donaldson
 * MIT License (MIT)
 * www.neopixel-controller.willdonaldson.io
 */

#include <Adafruit_NeoPixel.h>

#define LEDpin D7
#define LEDcount 216

Adafruit_NeoPixel strip1(LEDcount, LEDpin, NEO_GRB + NEO_KHZ800);

// arrays to store variables for 4 seperate sections of LED strips
// break 12 columns into 4 seperate sections (ensures rain pattern is evenly distributed across entire area)
int columnIndex[4];
int stepIndex[4];
int glow[4];
bool Running[4]={false, false, false, false};
int stepsTillNext[4] = {0, 9, 5, 14}; // seed at intervals of 4 or 5 LEDs spacing
float intensity = 1.0; // varible for LED brightness in range [0.0, 1.0]

int red[3] = {255, 0, 0};
int green[3] = {0, 255, 0};
int blue[3] = {0, 0, 255};
int cyan[3] = {0, 255, 255};
int magenta[3] = {255, 0, 51};
int yellow[3] = {255, 140, 0};
int violet[3] = {153, 0, 255};

int* colours[9];
int *selected_colour[4];

int colourIndex = 0;

void map_colours(){
  colours[0] = red;
  colours[1] = green;
  colours[2] = blue;
  colours[3] = violet;
  colours[4] = cyan;
  colours[5] = magenta;
  colours[6] = yellow;
}

void setup() {
  strip1.begin();           // initialize neopixel strip 
  strip1.setBrightness(255);
  strip1.show();  
  map_colours();
}

void movingPixel(int i, int col, int colour[3], int backgroundGlow = 0){
  int minLED = col * 18;
  int maxLED = (col+1) * 18 - 1;  

  // fade in/out function
  if(i < 2){ intensity = (i+1) * 0.5; }
  else if(i > 15){ intensity = (18 - i) * 0.5; }
  else{ intensity = 1.0; }

  int Red = (int)intensity*colour[0];
  int Green = (int)intensity*colour[1];
  int Blue = (int)intensity*colour[2];

  int RedGlow = (int)backgroundGlow*colour[0]/255;    // either 1 or 0
  int GreenGlow = (int)backgroundGlow*colour[1]/255;
  int BlueGlow = (int)backgroundGlow*colour[2]/255;
  
  if((col % 2) == 0){    // even column (drive in forward order)
    int pixel = maxLED - i;
    int prevPixel = pixel + 1;
    if(i == 18){ strip1.setPixelColor(prevPixel, RedGlow, 1, 0);}   // turn off last LED in chain
    else{
      strip1.setPixelColor(pixel, Red, Green, Blue);
      if(prevPixel <= maxLED){ strip1.setPixelColor(prevPixel, RedGlow, GreenGlow, BlueGlow);}   // turn the previous (trailing) LED off
    }  
  }
  else{   // odd column  (drive in reverse order)
    int pixel = minLED + i;
    int prevPixel = pixel - 1;
    if(i == 18){ strip1.setPixelColor(prevPixel, RedGlow, GreenGlow, BlueGlow);}   // turn off last LED in chain
    else{
      strip1.setPixelColor(pixel, Red, Green, Blue);
      if(prevPixel >= minLED){ strip1.setPixelColor(prevPixel, RedGlow, GreenGlow, BlueGlow);}   
    }                          
  }
}

void loop() {  
  for(int i=0; i<4; i++){
    if(stepIndex[i] > stepsTillNext[i]){
      Running[i] = true;
      stepsTillNext[i] = 18;  // only required on the first cycle to reset initial seed values
      columnIndex[i] = random((i*3), ((i+1)*3)); 

      //uncomment for green traditional Matrix text
      selected_colour[i] = green;

      //uncomment for rainbox text
      //selected_colour[i] = colours[colourIndex];
      //colourIndex += 1; 
      //if(colourIndex > 6){ colourIndex = 0; }     
      
      glow[i] = random(0, 2);     //optionally hardcode either 1 or 0 for background glow or not, respectively    
      stepIndex[i] = 0;
    }
    if(Running[i] == true){
      movingPixel(stepIndex[i], columnIndex[i], selected_colour[i], glow[i]);
      if(stepIndex[i] == 18){
        Running[i] = false;
      }
    }
    stepIndex[i] += 1;
  }
  strip1.show();
  delay(130);
}
