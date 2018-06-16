#include "treino.h"

/********************

   TREINAMENTO

 ********************/

//globais publicas
volatile bool startNow = 0;
uint8_t tamPiscina;
int NUMPIXELS; //int NUMPIXELS = tamPiscina * 10;
uint8_t qtd_serie = 0;
uint8_t qtdIda = 0;
uint8_t qtdVolta = 0;
uint8_t estado;
char buffer[250];
volatile uint16_t contGlobal = 0;
Serie serieAtual;


//globais privadas
Adafruit_NeoPixel fita;
char direcao = 'i'; //variavel que determina se é ida ou volta ------> 'i' = ida e 'v' = volta
uint16_t ultima_pos = 0;



void treino() {

//  fita.clear();
//  fita.show();
//  delay(10);
  configNeopixel();
  digitalWrite(TRANSISTOR, HIGH);

  //mudanca de estado
  if (buffer[contGlobal] == '\0') {
    estado = LEITURA; //se acabar todas as series enviadas pelo app, esperar nova resposta
  }
  else {
    estado = ATRIBUICAO; //senao, atribuir a prox serie
  }

  //**************logica do treino**************//

  int NUM_PISCINAS;

  sinaliza_comeco();
  for (int k = 0; k < serieAtual.repeticao; k++) {

    //    Serial.print(F("*****************REPETICAO****************    "));
    //    Serial.println(k + 1);

    NUM_PISCINAS = (serieAtual.distancia) / tamPiscina; //smp inteiro o NUM_PISCINAS, ja q a distancia é múltiplo do tamPiscina
    for (int j = 0; j < NUM_PISCINAS; j++) {

      if (j % 2 == 0) {
        //manda as piscinas de ida
        velocidade_constante(0);
      } else {
        //manda as piscinas de volta
        velocidade_constante(1);
      }
      //delay(500);//tirar delay???
    }
    //se for a ultima repeticao nao ha intervalo entre as repeticoes, ou seja, nao entra no if
    if (k != (serieAtual.repeticao) - 1) {

      int tempo_aux = serieAtual.descanso_distancia * 1000;
      if (tempo_aux >= 5000) {
        delay(tempo_aux - 5000);
        sinaliza_comeco();
      }
      else {
        //delay(tempo_aux); //lembrar de definir no app o tempo maior que cinco se houver intervalo, para tirar esse if else
        sinaliza_comeco();
      }
    }
  }

  int tempo_aux = serieAtual.descanso_serie * 1000;
  int offset;

  //intervalo entre series
  if (tempo_aux >= 5000) {
    if (estado == LEITURA) offset = 0;
    else offset = 5000;
  }
  else offset = 0;

  unsigned long tempo = millis();
  while (tempo_aux - offset >= millis() - tempo && startNow == 0) {
    //espera pelo tempo determinado pelo usuario ou por um comando para iniciar no momento desejado (StartNow)
    delay(100); //??
  }
  startNow = 0;
  //delay(tempo_aux - 5000);

  //  Serial.print(F("Memoria treino = "));
  //  Serial.println(freeRam());
  delete &fita;

}


void velocidade_constante(bool flag) {

   int pos_inicial, pos_final;
  uint8_t cor = 0;
  uint8_t sz;

  if (flag == 0) {
    sz = qtdIda;
    for (int j = 1; j <= sz; j++) {
      float aux = (serieAtual.ida.tempo[j - 1] * 1000) / (serieAtual.ida.trecho[j - 1] * 10); //conversao do tamanho para pixels (multiplicar por 10);
      uint16_t delayMs = aux;
      uint16_t delayUs = ((float)(aux - delayMs)) * 1000;
      //Serial.println(delayMs);
      //Serial.println(delayUs);
      
      calculo_pos(serieAtual.ida.trecho[j - 1], &pos_inicial, &pos_final);

      if (j != 1) {
        cor = comparaCor(serieAtual.ida.trecho[j - 2], serieAtual.ida.tempo[j - 2] * 1000, serieAtual.ida.trecho[j - 1], serieAtual.ida.tempo[j - 1] * 1000);
      }
      else {
        cor = 2; //Significa que esta no começo do trecho, ou seja sempre começa com verde
      }

      uint32_t pixel_color;
      if (cor == 0)
        pixel_color = fita.Color(0, 255, 0); //azul, caso lento
      else if (cor == 1)
        pixel_color = fita.Color(255, 0, 0); //vermelho, caso rapido
      else
        pixel_color = fita.Color(0, 0, 255); //verde, inicio da piscina
        
      ledControl(delayMs, delayUs, pos_inicial, pos_final, pixel_color);
    }
  }
  else {
    sz = qtdVolta;
    for (int j = 1; j <= sz; j++) {
      float aux = (serieAtual.volta.tempo[j - 1] * 1000) / (serieAtual.volta.trecho[j - 1] * 10); //conversao do tamanho para pixels (multiplicar por 10);
      uint16_t delayMs = aux;
      uint16_t delayUs = ((float)(aux - delayMs)) * 1000;
      //Serial.println(delayMs);
      //Serial.println(delayUs);

      calculo_pos(serieAtual.volta.trecho[j - 1], &pos_inicial, &pos_final);

      if (j != 1) {
        cor = comparaCor(serieAtual.volta.trecho[j - 2], serieAtual.volta.tempo[j - 2] * 1000, serieAtual.volta.trecho[j - 1], serieAtual.volta.tempo[j - 1] * 1000);
      }
      else {
        cor = 2; //Significa que esta no começo do trecho, ou seja sempre começa com verde
      }

      uint32_t pixel_color;
      if (cor == 0)
        pixel_color = fita.Color(0, 255, 0); //azul, caso lento
      else if (cor == 1)
        pixel_color = fita.Color(255, 0, 0); //vermelho, caso rapido
      else
        pixel_color = fita.Color(0, 0, 255); //verde, inicio da piscina

      ledControl(delayMs, delayUs, pos_inicial, pos_final, pixel_color);
    }
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


  //  Serial.print(F("Memoria comparaCor = "));
  //  Serial.println(freeRam());
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

  //  Serial.print(F("Memoria calculoPos = "));
  //  Serial.println(freeRam());

}

//funcao que controla a ida e volta dos leds. A flag define se controla a ida ou a volta
void ledControl(uint16_t delayMs, uint16_t delayUs, int pos_inicial, int pos_final, uint32_t cor) {

  if (direcao == 'i') {

    for (int i = pos_inicial; i < pos_final; i++) {


      fita.setPixelColor(i, cor); //define a cor do pixel i


      if (i != 0) {

        fita.setPixelColor(i - 1, fita.Color(0, 0, 0));

        if (i == NUMPIXELS - 1) {
          direcao = 'v';
        }

      }

      fita.show(); // This sends the updated pixel color to the hardware.

      delay(delayMs); // Delay for a period of time (in milliseconds).
      if (delayUs) delayMicroseconds(delayUs);

    }
  }


  else {
    for (int i = pos_inicial; i >= pos_final; i--) {

      fita.setPixelColor(i, cor); //define a cor do pixel i

      if (i != NUMPIXELS - 1) {

        fita.setPixelColor(i + 1, fita.Color(0, 0, 0));

        if (i == 0) {
          direcao = 'i';
        }

      }

      fita.show(); // This sends the updated pixel color to the hardware.

      delay(delayMs); // Delay for a period of time (in milliseconds).
      if (delayUs) delayMicroseconds(delayUs);

    }
  }

  //  Serial.print(F("Memoria ledControl = "));
  //  Serial.println(freeRam());

}


//funcao que sinaliza o começo de uma piscina. Primeiro acende-se os primeiros 2m de led e vai diminuindo a quantidade de leds acesos
//de 0.5 em 0.5m a cada 1 segundo. No total são 5 segundos para o começo do treino.
void sinaliza_comeco() {
  if (direcao == 'i') {

    for (int i = 19; i >= 0; i--) {
      fita.setPixelColor(i, fita.Color(0, 0, 255));
    }
    fita.show();
    pause();
    delay(1000);
    int j = 19;
    int aux;
    for (int i = 0; i < 4; i++) {
      aux = j - 5;
      for (; j > aux; j--) {
        fita.setPixelColor(j, fita.Color(0, 0, 0));
        fita.show();
      }
      delay(1000);
    }
  }

  else {
    for (int i = (NUMPIXELS - 20); i <= NUMPIXELS - 1; i++) {
      fita.setPixelColor(i, fita.Color(0, 0, 255));
    }
    fita.show();
    pause();
    delay(1000);
    int j = (NUMPIXELS - 20);
    int aux;
    for (int i = 0; i < 4; i++) {
      aux = j + 5;
      for (; j < aux; j++) {
        fita.setPixelColor(j, fita.Color(0, 0, 0));
        fita.show();
      }
      delay(1000);
    }
  }

  //  Serial.print(F("Memoria sinalizaComeco = "));
  //  Serial.println(freeRam());

}

void configNeopixel() {
  tamPiscina = readEEPROM();
  NUMPIXELS = tamPiscina * 10;
  fita = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);

  fita.begin();
  fita.clear();
  fita.show(); // Como nenhum pixel foi iniciado, esse comando apaga todos os led
  fita.setBrightness(10);   // variar luminosidade de todos os leds 0 - 255. So tem efeito dps que a funçao show eh chamada


}

