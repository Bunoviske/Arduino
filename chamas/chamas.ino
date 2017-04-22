
#include<SoftwareSerial.h>

SoftwareSerial BT (2, 3); //RX,TX

// Definicoes dos pinos ligados ao sensor
int pin_a0 = A0;

String mac = "98d3:31:308277";

int maximo = 500;

void setup()
{
  BT.begin(9600);
  Serial.begin(9600);
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);

}

void loop()
{
  /*
     SENSOR DE CHAMAS - FOGO

  */
  int nivelchama = analogRead(pin_a0);


  Serial.print(" Nivel de chama : ");
  Serial.println(nivelchama);
  BT.print(" Valor lido pelo sensor : ");
  BT.println(nivelchama);
  
  if (nivelchama < maximo) {
    digitalWrite(12, HIGH);
    delay(3000);
    digitalWrite(12, LOW);

  }
  delay(10000);

}

