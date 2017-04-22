#include <SoftwareSerial.h>

SoftwareSerial BT (2, 3);

int pir = 5;
int rele = 6;

String mac = "98:D3:31:20:8E:F4";

void setup() {
  // put your setup code here, to run once:
  pinMode(pir, INPUT);
  pinMode(rele, OUTPUT);
  Serial.begin(9600);
  BT.begin(9600);
  digitalWrite(rele, LOW);
}

bool read_pin( byte sensor)
{
  return digitalRead(sensor);
}


void loop() {

  //SE DETECTAR PRESENCA, AVISA PARA O SERVIDOR
  if (read_pin(pir) == 1) {
    BT.println("pessoa_detectada");
    Serial.println("pessoa_detectada");
    digitalWrite(rele, HIGH);
    delay(5000);
    digitalWrite(rele, LOW);


  } else {
    BT.println("pessoa_nao_detectada");
    Serial.println("pessoa_nao_detectada");
    delay(500);

  }


}
