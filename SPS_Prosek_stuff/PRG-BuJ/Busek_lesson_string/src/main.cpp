#include <Arduino.h>
String prikaz = "";

void vytiskni_menu() {
  Serial.println("Seznam Prikazu");
  Serial.println(" LED ON - zapne LED");
  Serial.println(" LED OFF - vypne LED");
}

void setup() {
  Serial.begin(9600);
  pinMode(12, OUTPUT);
  vytiskni_menu();
}

void loop() {
  if (Serial.available() > 0) {
    char znak = Serial.read();
  }

  for (int i; i < pocet_bliknuti; i++) {
    digitalWrite(13, 1);
    delay(250);
    digitalWrite(13, 0);
    delay(250);
  }
}