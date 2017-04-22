unsigned long var = 0;

void setup() {
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(2), pause, LOW);
}

void loop() {
  oi();
  if ( var > 1000 ) {
    detachInterrupt(digitalPinToInterrupt(2));

  }
}

void pause() {
  Serial.println("ISR");
  var++;

}

void oi() {
  delay(1000);
  Serial.println(var);
}

