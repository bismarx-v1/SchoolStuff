/*
  ReadAnalogVoltage

  Reads an analog input on pin 0, converts it to voltage, and prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/ReadAnalogVoltage
*/
// the setup routine runs once when you press reset:
/*
#define POT 7
#include <Arduino.h>
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(POT);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (3.3 / 1023.0);
  // print out the value you read:
  Serial.println(voltage);
}
*/

#define POT 7
#define LED 13


void setup(){
  pinMode(LED, OUTPUT);x
  //Serial.begin(9600);
}

void loop(){
  int dilky = analogRead(POT);
  static int stav_led = 0;
  
  float interval = 20.0 + 1980.0 * dilky/20.0;
  stav_led = !stav_led;
  digitalWrite(LED, stav_led);
 
  delay(interval);
  }