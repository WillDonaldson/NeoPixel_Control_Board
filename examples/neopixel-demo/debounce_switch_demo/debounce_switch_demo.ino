/* 
 * Demo of how to use interrupt service routine (ISR) with momentary switch and debounce signal
 * Momentary switch is connected to external pullup resistor
 * 
 * Copyright (c) 2021 Will Donaldson
 * MIT License (MIT)
 * www.neopixel-controller.willdonaldson.io
 */

#define modePin D5

static unsigned long t_Prev = 0;
int debounce_delay = 200;
int button_presses = 0;

void ICACHE_RAM_ATTR buttonISR();

void setup(){
  Serial.begin(9600);
  pinMode(modePin, INPUT);   // uses external pullup resistor
  attachInterrupt(digitalPinToInterrupt(modePin), buttonISR, FALLING);
}

void loop(){
  Serial.println(button_presses);
  delay(50);
}

void buttonISR(){
  unsigned long t_Now = millis();
  if(t_Now - t_Prev > debounce_delay){
    button_presses += 1;
  }
  t_Prev = t_Now;
}
