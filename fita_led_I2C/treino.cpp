#include "treino.h"

/********************

   TREINAMENTO

 ********************/


void treino() {

  int NUM_PISCINAS;
  for (int i = 0; i < qtd_serie + 1; i++) {
    sinaliza_comeco();
    for (int k = 0; k < series[i].repeticao; k++) {

      Serial.print(F("*****************REPETICAO****************    "));
      Serial.println(k + 1);

      NUM_PISCINAS = (series[i].distancia) / tamPiscina; //smp inteiro o NUM_PISCINAS, ja q a distancia é múltiplo do tamPiscina
      for (int j = 0; j < NUM_PISCINAS; j++) {

        if (j % 2 == 0) {
          //manda as piscinas de ida
          velocidade_constante(series[i].ida.tempo, series[i].ida.trecho, 0);
        } else {
          //manda as piscinas de volta
          velocidade_constante(series[i].volta.tempo, series[i].volta.trecho, 1);
        }
        //delay(500);//tirar delay???
      }
      //se for a ultima repeticao nao ha intervalo entre as repeticoes, ou seja, nao entra no if
      if (k != (series[i].repeticao) - 1) {

        int tempo_aux = series[i].descanso_distancia * 1000;
        if (tempo_aux >= 5000) {
          delay(tempo_aux - 5000);
          sinaliza_comeco();
        }
        else {
          delay(tempo_aux); //lembrar de definir no app o tempo maior que cinco se houver intervalo, para tirar esse if else
        }

      }

    }
    //intervalo entre series
    int tempo_aux = series[i].descanso_serie * 1000;
    if (tempo_aux >= 5000) {
      unsigned long tempo = millis();
      while (tempo_aux - 5000 >= millis() - tempo && startNow == 0) {
        //espera pelo tempo determinado pelo usuario ou por um comando para iniciar no momento desejado (StartNow)
        //delay(100); ??
      }
      startNow = 0;
      //delay(tempo_aux - 5000);
    }
  }

  if (buffer[contGlobal] == '\0') {
    estado = LEITURA; //se acabar todas as series enviadas pelo app, esperar nova resposta
  }
  else {
    estado = ATRIBUICAO; //senao, atribuir a prox serie
  }

  Serial.print(F("Memoria treino = "));
  Serial.println(freeMemory());
}


void velocidade_constante(float tempo[], float trecho[], bool flag) {

  int pos_inicial, pos_final;
  uint8_t cor = 0;
  uint8_t sz;

  if (flag == 0) {
    sz = qtdIda;
  }
  else {
    sz = qtdVolta;
  }


  for (int j = 1; j <= sz; j++) {

    Serial.print(F("Trecho: "));
    Serial.println(j);
    Serial.print(F("Tempo: "));
    Serial.println(tempo[j - 1]);

    int delayval = (tempo[j - 1] * 1000) / (trecho[j - 1] * 10); //conversao do tamanho para pixels (multiplicar por 10);
    calculo_pos(trecho[j - 1], &pos_inicial, &pos_final);

    if (j != 1) {
      cor = comparaCor(trecho[j - 2], tempo[j - 2] * 1000, trecho[j - 1], tempo[j - 1] * 1000);
    }
    else {
      cor = 2; //Significa que esta no começo do trecho, ou seja sempre começa com verde
    }

    uint32_t pixel_color;

    switch (cor) {
      case 0:
        //pixel_color = fita.Color(0, 150, 0); //azul, caso lento
        break;

      case 1:
        //pixel_color = fita.Color(150, 0, 0); //vermelho, caso rapido
        break;

      case 2:
        //pixel_color = fita.Color(0, 0, 150); //verde, inicio da piscina
        break;
    }

    ledControl(delayval, pos_inicial, pos_final, pixel_color);

    Serial.print(F("Memoria velconstante = "));
    Serial.println(freeMemory());

  }
}


int comparaCor(float tam_ante, int tempo_ante, float tam_atual, int tempo_atual) {

  float vel_atual, vel_ante;
  //tempo_atual = (tempo_atual / 1000); //mudar aqui
  //tempo_ante = (tempo_ante / 1000);
  vel_atual = ((float)tam_atual / tempo_atual);
  vel_ante = ((float)tam_ante / tempo_ante);

  if ((vel_atual / vel_ante) > 1) {
    return 1;
  }
  else if ((vel_atual / vel_ante) < 1) { //otimizar se preciso
    return 0;
  }
  else {
    return 2;
  }


  Serial.print(F("Memoria comparaCor = "));
  Serial.println(freeMemory());
}


void calculo_pos(float tam_trecho, int *pos_inicial, int *pos_final) {

  int pixel = tam_trecho * 10;

  if (direcao == 'i') {

    *pos_inicial = ultima_pos;

    *pos_final = *pos_inicial + pixel;

    if ( *pos_final == NUMPIXELS) {
      ultima_pos = *pos_final - 1;

    }
    else {
      ultima_pos = *pos_final;
    }

  }
  else {

    *pos_inicial = ultima_pos;

    *pos_final = 1 + (*pos_inicial - pixel);

    if ( *pos_final == 0) {
      ultima_pos = *pos_final;

    }
    else {
      ultima_pos = *pos_final - 1;
    }
  }

  Serial.print(F("Memoria calculoPos = "));
  Serial.println(freeMemory());

}



//funcao que controla a ida e volta dos leds. A flag define se controla a ida ou a volta
void ledControl(int delayval, int pos_inicial, int pos_final, uint32_t cor) {

  if (direcao == 'i') {

    for (int i = pos_inicial; i < pos_final; i++) {


      fita[i] = CRGB::Blue;


      if (i != 0) {

        fita[i-1] = CRGB::Black;

        if (i == NUMPIXELS - 1) {
          direcao = 'v';
        }

      }

      FastLED.show(); // This sends the updated pixel color to the hardware.

      delay(delayval); // Delay for a period of time (in milliseconds).

    }
  }


  else {
    for (int i = pos_inicial; i >= pos_final; i--) {

      fita[i] = CRGB::Blue; //define a cor do pixel i

      if (i != NUMPIXELS - 1) {

        fita[i+1] = CRGB::Black;

        if (i == 0) {
          direcao = 'i';
        }

      }

      FastLED.show(); // This sends the updated pixel color to the hardware.

      delay(delayval); // Delay for a period of time (in milliseconds).

    }
  }

  Serial.print(F("Memoria ledControl = "));
  Serial.println(freeMemory());
}


//funcao que sinaliza o começo de uma piscina. Primeiro acende-se os primeiros 2m de led e vai diminuindo a quantidade de leds acesos
//de 0.5 em 0.5m a cada 1 segundo. No total são 5 segundos para o começo do treino.
void sinaliza_comeco() {
  FastLED.clear();
  if (direcao == 'i') {

    for (int i = 19; i >= 0; i--) {
      fita[i] = CRGB::Blue;
    }
    FastLED.show();
    pause();
    delay(1000);
    int j = 19;
    int aux;
    for (int i = 0; i < 4; i++) {
      aux = j - 5;
      for (; j > aux; j--) {
        fita[j] = CRGB::Black;
        FastLED.show();

      }
      delay(1000);
    }
  }

  else {
    for (int i = (NUMPIXELS - 20); i <= NUMPIXELS - 1; i++) {
      fita[i] = CRGB::Blue;
      
    }
    FastLED.show();
    pause();
    delay(1000);
    int j = (NUMPIXELS - 20);
    int aux;
    for (int i = 0; i < 4; i++) {
      aux = j + 5;
      for (; j < aux; j++) {
        fita[j] = CRGB::Black;
        FastLED.show();
      }
      delay(1000);
    }
  }

  Serial.print(F("Memoria sinalizaComeco = "));
  Serial.println(freeMemory());

}
