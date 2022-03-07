/* NodeMCU ESP8266 boards only have one analog pin
 * By using a pair of IN4001 diodes you can toggle between two potnetiometers connected to the same analog pin
 * See wiring diagram in documentation
 * 
 * Note: for utilizing maximum range of potentiometer you must compensate in software for the 0.7V drop across IN4001 diode 
 * 
 * Copyright (c) 2021 Will Donaldson
 * MIT License (MIT)
 * www.neopixel-controller.willdonaldson.io
 */

#define analogPin A0    
#define pot1_enable D0  
#define pot2_enable D8

int pot1_value = 0; 
int pot2_value = 0; 

void setup() {
  Serial.begin(9600);
  pinMode(analogPin, INPUT);
  pinMode(pot1_enable, OUTPUT);
  pinMode(pot2_enable, OUTPUT);
  digitalWrite(pot1_enable, LOW);         // disengage both potentiometers
  digitalWrite(pot2_enable, LOW);
}

int read_pot1(){
  digitalWrite(pot1_enable, HIGH);        // temporarily engage one potentiometer on
  int val = analogRead(analogPin);
  digitalWrite(pot1_enable, LOW);         // disengage potentiometer off again
  return rescale_analog(val, 0, 255);
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
  int rescaled_val = constrain(map(val, 10, 990, Min, Max), Min, Max); 
  return rescaled_val;
}

void loop() {
  pot1_value = read_pot1();
  pot2_value = read_pot2();
  Serial.print("Potentiomer 1: ");
  Serial.print(pot1_value);
  Serial.print("\tPotentiomer 2: ");
  Serial.print(pot2_value);
  Serial.println();
  delay(100);
}
