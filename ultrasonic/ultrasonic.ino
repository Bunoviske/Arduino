#include <Ultrasonic.h>

#define TRIGGER_esq 9
#define ECHO_esq 8
#define TRIGGER_dir 7
#define ECHO_dir 6

Ultrasonic ultrasonic_esq(TRIGGER_esq, ECHO_esq);
Ultrasonic ultrasonic_dir(TRIGGER_dir, ECHO_dir);

int entra=0;
float distance_esq, distance_dir, menor_esq, menor_dir;
long microsec_esq;
long microsec_dir;
unsigned long tempo=0;

void ledireita(){
  microsec_dir = ultrasonic_dir.timing();
  distance_dir = ultrasonic_dir.convert(microsec_dir, Ultrasonic::CM);
}

void leesquerda(){
  microsec_esq = ultrasonic_esq.timing();
  distance_esq = ultrasonic_esq.convert(microsec_esq, Ultrasonic::CM);
}


void setup()
  {
  
  Serial.begin(9600);
 /* ledireita();
  leesquerda();
  ledireita();
  leesquerda();
  menor_esq = 4000;
  menor_dir = 4000;
  byte i;
  for (i=0;i<20;i++){
    ledireita();
    leesquerda();
    if (distance_esq < menor_esq ){
      menor_esq = distance_esq;
    }
    if (distance_dir < menor_dir){
      menor_dir = distance_dir;
    }
  }
  Serial.println("Distancia padrao encontrada; pronto para iniciar");
  Serial.print("Esquerda: ");
  Serial.print(menor_esq);
  Serial.println(" cm");
  Serial.print("Direita: ");
  Serial.print(menor_dir);
  Serial.println(" cm");*/
  }

void loop()
  {
 // leesquerda();
  ledireita();
  Serial.println(distance_dir);
  delay(100);
  /*if (distance_esq < menor_esq - 5 ){
    
    while(distance_esq < menor_esq - 5){
      leesquerda();
    }
    tempo = millis();
    while ((millis()-tempo) < 300){
      ledireita();
      if (distance_dir < menor_dir - 5){
         while (distance_dir < menor_dir - 5){
          ledireita();
      }
      entra++;
      delay(200);
      Serial.println(entra);
      tempo = -600;
      }
    }
  }
  else if (distance_dir < menor_dir - 5){
    
    while(distance_dir < menor_dir - 5){
      ledireita();
    }  
    tempo = millis();
    while ((millis()-tempo) < 300){
        leesquerda();
        if (distance_esq < menor_esq - 5){
           while (distance_esq < menor_esq - 5){
            leesquerda();
           }
           entra--;
           delay(200);
           Serial.println(entra);
           tempo = -600;
      }
    }
  }*/
}
