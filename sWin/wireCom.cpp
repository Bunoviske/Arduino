#include "wireCom.h"

static uint8_t cont = 0; //global local: index do buffer do treino

void configWire() {
  //Wire.setClock(10000);
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

  bufferInterrupt[0] = Wire.read(); //caso o primeiro caracter enviado nao seja caracter de referencia do treino, armazena no buffer auxiliar
  i++;

  /************************recebimento do treino**************************/
  if (bufferInterrupt[0] == '&') { //primeiro caracter de uma serie
    cont = 0;
    while (Wire.available() && continua) {
      char c = Wire.read();
      if ( c != '&') {
        sb::buffer[cont] = c;
        cont++;
      }
      else {
        sb::buffer[cont] = '\0';
        //Serial.println('0');
        sb::received = 1;
        continua = 0;
        Serial.println(sb::buffer);
      }
    }
    //Serial.println('1');
  }
  else if (bufferInterrupt[0] == '%') { //caracter no meio da serie, devido a quebra de 32 bytes de escrita por vez, entao nao zera o contador
    while (Wire.available() && continua) {
      char c = Wire.read();
      if ( c != '&') {
        sb::buffer[cont] = c;
        cont++;
      }
      else {
        sb::buffer[cont] = '\0';
        //Serial.println('0');
        sb::received = 1;
        continua = 0;
        Serial.println(sb::buffer);
      }
    }
  }
  /************************recebimento do treino**************************/


  //recebimento do pause/stop
  else if (howmany == 5) {
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
        sb::NUMPIXELS = atoi(bufferInterrupt) * 10; //tamPiscina * 10
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

