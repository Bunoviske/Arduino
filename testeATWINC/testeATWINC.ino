
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  mySerial.begin(115200);
  

//  pinMode(8,OUTPUT);
//  pinMode(9,OUTPUT);
//  pinMode(10,OUTPUT);
//  pinMode(11,OUTPUT);
//
//  digitalWrite(8, HIGH);
//  digitalWrite(9, HIGH);
//  digitalWrite(10, HIGH);
//  digitalWrite(11, HIGH);
//
//  delay(1000);
//  Serial.println("AT\r\n");
}

void loop() { // run over and over
  if (mySerial.available()) {
    //mySerial.write("ok");
    Serial.print((char)mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write("<ok>");
  }
}

