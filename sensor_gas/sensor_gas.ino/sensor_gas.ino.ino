#include<SoftwareSerial.h>

SoftwareSerial BT (2,3); //RX,TX

// Definicoes dos pinos ligados ao sensor
int pin_d0 = 7;
int pin_a0 = A2;

String mac = "98d3:31:407bb4";

int nivelsensor=250;

void setup()
{
  BT.begin(9600);
  Serial.begin(9600);
  pinMode(pin_d0, INPUT);
}

void loop()
{
  /*
   * MQ-7
   * 
   */
  int valor_digital = digitalRead(pin_d0);
  int mq7 = analogRead(pin_a0);
  int mq2 = analogRead(A1);

  Serial.print(" MQ-7 : ");
  Serial.println(mq7);
  BT.print(" Valor lido pelo sensor : ");
  BT.println(mq7);
  if(mq7>nivelsensor){
    BT.print("Nível de gás acima do seguro");
  }
  delay(1000);
    
  /*
   * MQ-2
   * 
   */
  Serial.print(" MQ-2 : ");
  Serial.println(mq2);
  BT.print(" Valor lido pelo sensor : ");
  BT.println(mq2);

  delay(10000);
}

