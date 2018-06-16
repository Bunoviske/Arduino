#include "wireCom.h"

void configWire() {
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(&receiveEvent);
  Wire.onRequest(&requestEvent);
}

//FUNCOES DE CALLBACK

//funcao de callback que recebe dados do master
void receiveEvent(int howmany) {
  uint8_t continua = 1;
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
          sb::goPause = !sb::goPause;
        }
        else if (atoi(bufferInterrupt) == 4444) {
          sb::subState = STOP;
        }
        else if (atoi(bufferInterrupt) == 1111) {
          sb::startNow = 1;
        }
        else if (atoi(bufferInterrupt) == 2222) {
          sb::goSleep = 1;
        }
      }
    }
  }
  //caso em que recebe o tamanho da pisicina, que smp deve conter 2 ou menos digitos
  else if (howmany <= 4) {
    while (Wire.available() && continua) {
      char c = Wire.read();
      if ( c != '&') {
        bufferInterrupt[i] = c;
        i++;
      }
      else {
        bufferInterrupt[i] = '\0';
        sb::NUMPIXELS = atoi(bufferInterrupt)*10; //tamPiscina * 10
        continua = 0;
      }
    }
  }

  //recebimento do treino
  else {
    uint16_t cont = 0;
    while (Wire.available() && continua) {
      char c = Wire.read();
      if ( c != '&') {
        sb::buffer[cont] = c;
        cont++;
      }
      else {
        sb::buffer[cont] = '\0';
        //Serial.println(buffer);
        sb::received = 1;
        continua = 0;
      }
    }
  }
}

//funcao callback que eh chamada quando o master pede para o slave mandar algo
void requestEvent()
{
  if (sb::sendState == 1) {
    Wire.write("555"); //estado de receber o treino
    //Wire.write(freeMemory());
  }

}

