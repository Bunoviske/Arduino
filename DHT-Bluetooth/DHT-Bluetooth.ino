
#include <DHT.h>
#include<SoftwareSerial.h>
 
SoftwareSerial BT (2,3); //RX,TX
 
#define dht_pin 7
 
#define DHTTYPE DHT11
 
DHT dht(dht_pin, DHTTYPE);

String mac = "98d3:31:404ecb";
 
void setup()
{
  Serial.begin(9600);
  BT.begin(9600);
  delay(1000);
  dht.begin();
}
 
void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  // Mostra os valores lidos, na serial
  Serial.print("Temp. = ");
  Serial.print(t);
  Serial.print(" C ");
  Serial.print("Um. = ");
  Serial.print(h);
  Serial.println(" % ");
  BT.println(t);
  BT.println(h);
  delay(10000);
}
