#include <SoftwareSerial.h>

SoftwareSerial BT (2, 3);

int ldr = A0;
int laser = 7;

String mac = "98:D3:31:20:8E:F4";

void setup() {
  // put your setup code here, to run once:
  pinMode(ldr, INPUT);
  pinMode(laser, OUTPUT);
  Serial.begin(9600);
  BT.begin(9600);
  analogWrite(laser, 100);
}


void loop() {

 int ldr = analogRead(ldr);

 Serial.println(ldr);

 if (ldr < 20 ){
  Serial.println("Laser detectado");
  BT.println("laser_detectado");
 }
 else{
  BT.println("laser_nao_detectado");
 }

 delay(10000);


}
