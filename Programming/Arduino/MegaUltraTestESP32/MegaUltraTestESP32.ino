void setup() {
  pinMode(5, OUTPUT);
  pinMode(32, INPUT);
  Serial.begin(250000);
}

void loop() {
  digitalWrite(5, 1);
  Serial.println(digitalRead(32));
  delay(100);
  digitalWrite(5, 0);
  Serial.println(digitalRead(32));
  delay(100);
}
