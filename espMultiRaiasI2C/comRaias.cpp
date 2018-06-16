#include "comRaias.h"

//Inicializacao das variaveis globais
bool state[MaxRaias];
bool ligado[MaxRaias];

void configGPIO() {

  //ADD .RESERVE() PARA AS STRINGS QUE SERVEM COMO BUFFER! OU TROCAR PARA CHAR ARRAY

  definePins(); //define pinos de interrupcao

  for (uint8_t i = 0; i < MaxRaias; i++) {
    pinMode(arduinoPins[i], INPUT);
    state[i] = 0;
    interrupted[i] = 0;
    ligado[i] = 0;
    tempoTreino[i] = 0;
    tempoSerie[i] = 0;
    tempoSerieOffset[i] = 0;
    tempoInicial[i] = 0;
    tempoPause[i] = 0;
    lastDescansoSerie[i] = 0;
  }
}

void sleepAll() {
  Serial.println("Sleeping");
  for (uint8_t i = 0; i < MaxRaias; i++) {
    idRaia = i;
    data2send = "2222&";
    sendData2Raia(0);
  }
}

void readRaias() {

  for (uint8_t i = 0; i < MaxRaias; i++) {
    //so da request para as raias ligadas
    if (ligado[i] == 1) {

      //so da request para as raias que estao fora do treino
      if (checktempoSerie(i) == 1) {

        //Serial.println("checktempoSerie");

        Wire.requestFrom(i + 1, 3);
        delay(50);
        String retorno = "";
        while (Wire.available()) {
          char c = (char)Wire.read();
          retorno += c;
        }

        if (retorno.toInt() == 555) { //raia no estado de receber treino/serie
          Serial.println("Raia no estado de leitura");
          state[i] = 1;
          idRaia = i;
          uint8_t ret = getSerie();
          if (ret == 1) {
            //ha mais serie do mesmo treino para enviar à raia.
            sendData2Raia(1);
            state[i] = 0;
            waitForSignal2Desinterrupt();//funcao chamada apenas se for envio entre series, e nao no comeco de um novo treino.
          }
          else if (ret == 2) { //novo treino
            sendData2Raia(1);
            state[i] = 0;
            interrupted[idRaia] = 1; //primeira serie do treino, entao fica interrompido
          }
        }
        else {
          state[i] = 0;
        }
        //delay(100); // ?????
      }
    }
  }
}

void sendData2Raia(bool serie) {

  //int tam = data2send.length();
  const char* buffer = data2send.c_str();
  int tam = strlen(buffer);

  //Serial.println(tam);
  //Serial.println(buffer);

  int cont = 0;
  bool continua = 0;
  uint8_t i = 0;


  while (continua == 0) {
    Wire.beginTransmission(idRaia + 1);

    i = 0;
    if (serie == 1) {
      if (cont == 0) Wire.write('&');
      else Wire.write('%');
      i++;
    }

    for (; i < 32; i++) {
      if (cont < tam) {
        Serial.println(buffer[cont]);
        Wire.write(buffer[cont]);
        cont++;
      }
      else {
        i = 32; //sair do for
        continua = 1; //acabou de mandar toda a string
      }
    }
    Wire.endTransmission();
    delay(50);
  }
}

void waitForSignal2Desinterrupt() {

  Serial.println("Waiting signal");

  bool continua = 0;
  while (continua == 0) { //enquanto o arduino nao der o sinal para mandar desinterrupt, fica no loop.
    continua = digitalRead(arduinoPins[idRaia]);
  }

  data2send = "3333&"; //desinterrupt
  sendData2Raia(0);
  //changeInitialTime(); //como eh um desinterrupcao entre series, nao deve-se alterar o tempo inicial!!!!!!
  interrupted[idRaia] = 0; //desinterrompeu ---> redundante (tirar???)

  tempoSerieOffset[idRaia] = millis() - tempoInicial[idRaia];
  //necessario diminuir o tempo que comecou a proxima serie do treino, para que checktempoSerie() retorne o valor correto!!


}

