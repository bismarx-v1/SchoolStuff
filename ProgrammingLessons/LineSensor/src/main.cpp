#include <Arduino.h>
#include "LineSensor.h"
#define PIN_R 13
#define PIN_L 14

void setup() {
  Serial.begin(115200);
}

void loop() {

  Serial.print(line_sensing_get(PIN_R));
  delay(1000);
}