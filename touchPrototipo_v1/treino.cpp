#include "treino.h"

static unsigned long tempoMillis;

uint8_t train() {

  clearAll();

  int posAtual, posFinal;
  int nextPixel;
  int tempo;
  char direcaoInicial = gb::direcao;

  for (int i = 0; i < gb::qtdSeries; i++) {

    gb::serieAtual = i;
    sinalizaComeco();
    initRepeticao();

    for (int j = 0; j < 2 * gb::repeticoes; j++) {

      if (direcaoInicial == gb::direcao) { //contagem de repeticao no display
        updateRepeticao(((j + 1) / 2) + 1); //sem + 1
      }

      if (gb::direcao == 'd') {
        posAtual = 0;
        posFinal = MaxLeds - 1;
        nextPixel = 1;
        tempo = (gb::TimeIda * 1000) / (MaxLeds - 1);
      }
      else {
        posAtual = MaxLeds - 1;
        nextPixel = -1;
        posFinal = 0;
        tempo = (gb::TimeVolta * 1000) / (MaxLeds - 1);
      }
      digitalWrite(gb::leds[posAtual], HIGH);
      posAtual += nextPixel;

      while (posAtual != posFinal + nextPixel) {
        delayGym(tempo, false, 0);
        digitalWrite(gb::leds[posAtual], HIGH);
        if (posAtual - nextPixel >= 0 && posAtual - nextPixel < MaxLeds) digitalWrite(gb::leds[posAtual - nextPixel], LOW);
        posAtual += nextPixel;

        //indica que é a ultima repeticao
        if (j == (2 * gb::repeticoes) - 2 && direcaoInicial == 'd') digitalWrite(gb::leds[0], HIGH);
        else if (j == (2 * gb::repeticoes) - 2 && direcaoInicial == 's') digitalWrite(gb::leds[MaxLeds - 1], HIGH);

      }

      if (gb::TimeEspera && direcaoInicial == gb::direcao) { //tempo de isometria
        delayGym((uint32_t)gb::TimeEspera * 1000, false, 0);
      }

      //mudanca de direcao do movimento
      if (gb::direcao == 'd') gb::direcao = 's';
      else gb::direcao = 'd';

    }
    if (i < gb::qtdSeries - 1 ) {
      initDescanso();
      for (int k = gb::intervalo; k > MaxLeds; k--) {
        delayGym(1000, true, k);
      }
    }
  }

  clearAll();
  gb::subState = GET_USER_INFO_MAIN;
  gb::firstTime = 1;
  initTouch();
  return LEITURA;
}

void sinalizaComeco() {
  //  int segundos = ledsSinalizaComeco;
  //
  //  if (gb::direcao == 'd') {
  //    for (uint8_t i = 0; i < ledsSinalizaComeco; i++) {
  //      digitalWrite(gb::leds[i], HIGH);
  //    }
  //    int cont = ledsSinalizaComeco;
  //    while (cont != 0 ) {
  //      delayGym(1000, true, segundos);
  //      cont--;
  //      segundos--;
  //      digitalWrite(gb::leds[cont], LOW);
  //    }
  //    digitalWrite(gb::leds[cont], HIGH); //SOMENTE PARA EFEITOS DE VISUALIZACAO
  //  }
  //  else {
  //    for (uint8_t i = 0; i < ledsSinalizaComeco; i++) {
  //      digitalWrite(gb::leds[i], HIGH);
  //    }
  //    int cont = -1;
  //    while (cont != ledsSinalizaComeco - 1 ) {
  //      delayGym(1000, true, segundos);
  //      cont++;
  //      segundos--;
  //      digitalWrite(gb::leds[cont], LOW);
  //    }
  //    digitalWrite(gb::leds[cont], HIGH); //SOMENTE PARA EFEITOS DE VISUALIZACAO
  //  }
  int segundos = MaxLeds;

  if (gb::direcao == 'd') {
    for (uint8_t i = 0; i < MaxLeds; i++) {
      digitalWrite(gb::leds[i], HIGH);
    }
    initInicializaComeco();
    int cont = MaxLeds;
    while (cont != 0 ) {
      delayGym(1000, true, segundos);
      cont--;
      segundos--;
      digitalWrite(gb::leds[cont], LOW);
    }
    digitalWrite(gb::leds[cont], HIGH); //SOMENTE PARA EFEITOS DE VISUALIZACAO
  }
  else {
    for (uint8_t i = 0; i < MaxLeds; i++) {
      digitalWrite(gb::leds[i], HIGH);
    }
    initInicializaComeco();
    int cont = -1;
    while (cont != MaxLeds - 1 ) {
      delayGym(1000, true, segundos);
      cont++;
      segundos--;
      digitalWrite(gb::leds[cont], LOW);
    }
    digitalWrite(gb::leds[cont], HIGH); //SOMENTE PARA EFEITOS DE VISUALIZACAO
  }

}

void delayGym(uint32_t delayTime, bool printDisplay, int segundo) {
  tempoMillis = millis();

  if (printDisplay) {
    updateDescanso(segundo);
  }

  while (delayTime > millis() - tempoMillis) {
    //faça algo ou aguarde
  }
}

void clearAll() {
  for (uint8_t i = 0; i < MaxLeds; i++) {
    digitalWrite(gb::leds[i], LOW);
  }
}

