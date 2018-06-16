#include "time.h"

void train() {

  clearAll();

  int posAtual, posFinal;
  int nextPixel;
  int tempo;
  char direcaoInicial = direcao;

  for (int i = 0; i < qtdSeries; i++) {

    sinalizaComeco();

    for (int j = 0; j < 2 * repeticoes; j++) {

      if (direcao == 'd') {
        posAtual = 0;
        posFinal = MaxLeds - 1;
        nextPixel = 1;
        tempo = (TimeDescida * 1000) / (MaxLeds - 1);
      }
      else {
        posAtual = MaxLeds - 1;
        nextPixel = -1;
        posFinal = 0;
        tempo = (TimeSubida * 1000) / (MaxLeds - 1);
      }
      digitalWrite(leds[posAtual], HIGH);
      posAtual += nextPixel;

      while (posAtual != posFinal + nextPixel) {
        delay(tempo);
        digitalWrite(leds[posAtual], HIGH);
        if (posAtual - nextPixel >= 0 && posAtual - nextPixel < MaxLeds) digitalWrite(leds[posAtual - nextPixel], LOW);
        posAtual += nextPixel;

        //indica que é a ultima repeticao
        if (j == (2 * repeticoes) - 2 && direcaoInicial == 'd') digitalWrite(leds[0], HIGH);
        else if (j == (2 * repeticoes) - 2 && direcaoInicial == 's') digitalWrite(leds[MaxLeds - 1], HIGH);

      }

      if (TimeEspera && direcaoInicial == direcao) {
        delay(TimeEspera * 1000);
      }

      //mudanca de direcao do movimento
      if (direcao == 'd') direcao = 's';
      else direcao = 'd';

    }
    if (i < qtdSeries - 1 ) delay((intervalo - 9) * 1000);
  }

  clearAll();
}

void sinalizaComeco() {
  if (direcao == 'd') {
    for (uint8_t i = 0; i < MaxLeds; i++) {
      digitalWrite(leds[i], HIGH);
    }
    int cont = MaxLeds;
    while (cont != 0 ) {
      delay(1000);
      cont--;
      digitalWrite(leds[cont], LOW);
    }
  }
  else {
    for (uint8_t i = 0; i < MaxLeds; i++) {
      digitalWrite(leds[i], HIGH);
    }
    int cont = -1;
    while (cont != MaxLeds - 1 ) {
      delay(1000);
      cont++;
      digitalWrite(leds[cont], LOW);
    }
  }

}

void clearAll() {
  for (uint8_t i = 0; i < MaxLeds; i++) {
    digitalWrite(leds[i], LOW);
  }
}

