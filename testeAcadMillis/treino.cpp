#include "treino.h"

static unsigned long tempoMillis;

uint8_t treino() {

  clearAllLeds();

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
        posFinal = MAX_LEDS - 1;
        nextPixel = 1;
        tempo = (gb::TimeIda * 1000) / (MAX_LEDS - 1);
      }
      else {
        posAtual = MAX_LEDS - 1;
        nextPixel = -1;
        posFinal = 0;
        tempo = (gb::TimeVolta * 1000) / (MAX_LEDS - 1);
      }
      gb::fita.setPixelColor(posAtual, gb::fita.Color(255, 0, 0));
      gb::fita.show();
      posAtual += nextPixel;

      while (posAtual != posFinal + nextPixel) {
        delayGym(tempo, false, 0);
        gb::fita.setPixelColor(posAtual, gb::fita.Color(255, 0, 0));
        if (posAtual - nextPixel >= 0 && posAtual - nextPixel < MAX_LEDS) gb::fita.setPixelColor(posAtual - nextPixel, gb::fita.Color(0, 0, 0));
        posAtual += nextPixel;

        //indica que é a ultima repeticao
        if (j == (2 * gb::repeticoes) - 2 && direcaoInicial == 'd') gb::fita.setPixelColor(0, gb::fita.Color(255, 0, 0));
        else if (j == (2 * gb::repeticoes) - 2 && direcaoInicial == 's') gb::fita.setPixelColor(MAX_LEDS - 1, gb::fita.Color(255, 0, 0));

        gb::fita.show();

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
      for (int k = gb::intervalo; k > MAX_LEDS; k--) {
        delayGym(1000, true, k);
      }
    }
  }

  clearAllLeds();
  initTouch();
  return LEITURA;
}

void sinalizaComeco() {

  int segundos = ledsSinalizaComeco;
  
  initInicializaComeco();

  if (gb::direcao == 'd') {

    for (uint8_t i = 0; i < ledsSinalizaComeco; i++) {
      gb::fita.setPixelColor(i, gb::fita.Color(255, 0, 0));
    }
    gb::fita.show();

    int cont = ledsSinalizaComeco;
    while (cont != 0 ) {
      delayGym(1000, true, segundos);
      cont--;
      segundos--;
      gb::fita.setPixelColor(cont, gb::fita.Color(0, 0, 0));
      gb::fita.show();
    }
    gb::fita.setPixelColor(cont, gb::fita.Color(255, 0, 0)); //SOMENTE PARA EFEITOS DE VISUALIZACAO
    gb::fita.show();
  }
  else {

    for (uint8_t i = MAX_LEDS - ledsSinalizaComeco; i < MAX_LEDS; i++) {
      gb::fita.setPixelColor(i, gb::fita.Color(255, 0, 0));
    }
    gb::fita.show();

    int cont = -1;
    while (cont != ledsSinalizaComeco - 1 ) {
      delayGym(1000, true, segundos);
      cont++;
      segundos--;
      gb::fita.setPixelColor(cont, gb::fita.Color(0, 0, 0)); //SOMENTE PARA EFEITOS DE VISUALIZACAO
      gb::fita.show();
    }
    gb::fita.setPixelColor(cont, gb::fita.Color(255, 0, 0)); //SOMENTE PARA EFEITOS DE VISUALIZACAO
    gb::fita.show();
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

void clearAllLeds() {
  gb::fita.begin();
  gb::fita.show(); // Initialize all pixels to 'off'
  delay(100);
}

