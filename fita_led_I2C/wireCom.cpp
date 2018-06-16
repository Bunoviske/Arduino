#include "wireCom.h"

/********************

   COMUNICACAO COM ESP VIA I2C

 ********************/

void pega_treino() {

  contGlobal = 0;
  sendState = 1;

  while (received == 0) {
    delay(100);
  }
  Serial.println(F("Serie recebida"));
  //Debugg
  Serial.println(buffer);

  sendState = 0;
  contGlobal = 0;
  getTrainTime();

  contGlobal = 0;
  received = 0;

  estado = ATRIBUICAO;
}

//FUNCOES DE CALLBACK

//funcao de callback que recebe dados do master
void receiveEvent(int howmany) {
  bool continua = 1;
  char bufferInterrupt[7];
  uint8_t i = 0;

  //recebimento do pause/stop
  if (howmany == 5) {
    while (Wire.available() && continua) {
      char c = Wire.read();
      if ( c != '&') {
        bufferInterrupt[i] = c;
        i++;
      }
      else {
        bufferInterrupt[i] = '\0';

        //ANALISE DA RESPOSTA: PAUSE / STOP / RECEIVE TIME ACKNOWLEDGE /STARTNOW(entre intervalo de series) / SLEEP MODE

        if (atoi(bufferInterrupt) == 3333) {
          pause();
        }
        else if (atoi(bufferInterrupt) == 4444) {
          Stop();
        }
        else if (atoi(bufferInterrupt) == 1234) {
          rcvTime = 1;
        }
        else if (atoi(bufferInterrupt) == 1111) {
          startNow = 1;
        }
        else if (atoi(bufferInterrupt) == 2222) {
          sleep();
          while (startProgram == 0) { //arduino espera o esp mandar o tamanho da piscina para iniciar o codigo
            delay(100);
          }
          Stop();
        }
      }
    }
  }
  //caso em que recebe o tamanho da pisicina, que smp deve conter 2 ou menos digitos
  else if (howmany <= 4) {
    if (startProgram == 0) {
      while (Wire.available() && continua) {
        char c = Wire.read();
        if ( c != '&') {
          bufferInterrupt[i] = c;
          EEPROM.write(i, bufferInterrupt[i]);
          i++;
        }
        else {
          bufferInterrupt[i] = '\0';
          EEPROM.write(i, bufferInterrupt[i]);
          //tamPiscina = atoi(bufferInterrupt);
          startProgram = 1;
          continua = 0;
        }
      }
    }
  }

  //recebimento do treino
  else {
    while (Wire.available() && continua) {
      char c = Wire.read();
      if ( c != '&') {
        buffer[contGlobal] = c;
        contGlobal++;
      }
      else {
        buffer[contGlobal] = '\0';
        //Serial.println(buffer);
        received = 1;
        continua = 0;
      }
    }
  }
}

//funcao callback que eh chamada quando o master pede para o slave mandar algo
void requestEvent()
{
  if (sendState == 1) {
    Wire.write("555"); //estado de receber o treino
  }
  else if (sendTrainTime == 1) {
    String aux = "";
    aux.concat(tempo);
    aux += '&';
    Wire.write(aux.c_str());
  }
  else if (sendSeriesTime == 1) {
    //Wire.write();
  }
}

void configWire() {
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(&receiveEvent);
  Wire.onRequest(&requestEvent);
}

