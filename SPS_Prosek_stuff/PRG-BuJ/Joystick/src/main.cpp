#include <Arduino.h>
#define sw 4
#define LED 2

void setup() {
  Serial.begin(9600);
  pinMode(sw, INPUT);
  pinMode(LED, OUTPUT);


}

void loop() {
  int x = analogRead(15);
  //int y = analogRead(26);
  static int stav_sw; 
  Serial.println(x);
  Serial.println("\t");
  //Serial.println(y);
  

  

  delay(50);

}
