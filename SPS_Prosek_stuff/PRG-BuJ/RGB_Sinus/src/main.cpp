#include <Arduino.h>
#define LED 10

double heartbeat(double t) {
    // Parameters
    const double A = 1.5;  // Amplitude of the main spike
    const double B = 50.0; // Sharpness of the spike
    const double T0 = 1.0; // Time of the main spike
    const double C = 20.0; // Frequency of oscillations around the spike
    const double D = 0.2;  // Amplitude of the small oscillations
    const double E = 10.0; // Frequency of the smaller oscillations

    // Compute the heartbeat function
    return A * exp(-B * (t - T0) * (t - T0)) * sin(C * t) + D * sin(E * t);
}

void setup() {
  Serial.begin(115200);
}
/*
void loop() {
  float t = millis() % 1000 / 1000.0;
  float h = 255.0 * (heartbeat(t) + 1.0) / 2.5;
  analogWrite(LED, (int)h);
}
*/
  //float w = 2*PI;
  //float intenzita = 255.0 * (sin(t) + 1.0) / 2.0;
void loop() {
  int x = analogRead(A0);
  const float alpha = 0.9;
  static float f;
  f = alpha * x + (1.0-alpha)*f;

  Serial.print(x);
  Serial.print('\t');
  Serial.print(f);
  Serial.println();
  delay(50);
}