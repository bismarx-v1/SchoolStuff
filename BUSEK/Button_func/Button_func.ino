// Example Arduino program for reading the Pololu Distance Sensor with Pulse Width Output, 50cm Max


const uint8_t sensorPin = 5;
 
void setup()
{
  Serial.begin(115200);
}
 
void loop()
{
  int16_t t = pulseIn(sensorPin, HIGH);
 
  if (t == 0)
  {
    // pulseIn() did not detect the start of a pulse within 1 second.
    Serial.println("timeout");
  }
  else if (t > 1850)
  {
    // No detection.
    Serial.println(-1);
  }
  else
  {
    // Valid pulse width reading. Convert pulse width in microseconds to distance in millimeters.
    int16_t d = (t - 1000) * 3 / 4;
 
    // Limit minimum distance to 0.
    if (d < 0) { d = 0; } 
  
    Serial.print(d);
    Serial.println(" mm");
  }
}