#include <avr/pgmspace.h>
#include <SoftwareSerial.h>

/*
   LINK PARA INSTALACAO DA BIBLIOTECA
   https://github.com/adafruit/Adafruit_NeoPixel
*/
#include <Adafruit_NeoPixel.h>

/*
   LINK PARA INSTALACAO DA BIBLIOTECA
   https://github.com/McNeight/MemoryFree
*/
#include <MemoryFree.h>



/**************************************

   DEFINES

**************************************/

#define LEITURA        0
#define TREINO         1
#define ATRIBUICAO     2

#define PIN            6

#define INTERRUPT      2

#define aceleracao    -0.5

//Define o tamanho da fita em m
#define tamanho        5

//Tamanho dos vetores de series
#define TAM_MAX        1 //otimizar tamanho das variaveis para caber mais series e trocar o vetor dinamico da struct "Piscina" caso precise
//arduino mega aguenta TAM_MAX = 100 ocupando 46% de variavel global

//#define DEBUG


typedef struct {

  int trecho[5];
  int tempo[5];  //armazenar os tempos em segundos

} Piscina;

typedef struct {

  Piscina ida;
  Piscina volta;
  int repeticao;
  int distancia;
  int descanso_serie;  //armazenar os tempos em segundos
  int descanso_distancia;


} Serie;


/******

  Comentarios sobre o codigo

 ******/

//quando a distancia da serie for igual ao tamanho da piscina, a Piscina de ida e volta devem receber os mesmos dados ?????????????????????????? pensar!

//nao vai ter mais a struct de treinos. Só haverá series, entao a divisão em varios treinos deverá ser feito apenas no aplicativo. Assim, o tempo
//entre um treino e outro nada mais é que o tempo entre uma série e outra no arduino

//lista ou vetor estatico??

//testar progressivo e regressivo

//tratar no app para que seja no maximo n trechos ---> atualmente sao 5

/**************************************
                CODIGO
**************************************/

/*
   A logica do treino é dividida em series e piscinas. Cada serie é definida por uma certa quantidade de repeticoes
   e cada piscina tem como caracteristica trechos determinados pelo usuario e o tempo que ele deve percorrer
   em cada um deles. Além disso, é definido um intervalo entre cada distancia e o descanso entre cada
   serie.

*/

SoftwareSerial esp(3, 4);

const PROGMEM int NUMPIXELS = tamanho * 10; //está sendo considerado uma fita que tem 30 leds/m e que é alimentada por 12V (controle de 3 leds por vez)

const PROGMEM Adafruit_NeoPixel fita = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);

//qtd de series
uint8_t qtd_serie = 0; //para manter a parametrizacao, foi somado um ao valor de qtd_serie na funcao de treino

//qtd de trecho na ida e na volta - MAX 5 !!!
uint8_t qtdIda = 0;
uint8_t qtdVolta = 0;

char direcao = 'i'; //variavel que determina se é ida ou volta ------> 'i' = ida e 'v' = volta

uint16_t ultima_pos = 0; //variavel que auxilia o controle dos trechos de cada piscina

uint8_t estado;

//char buffer[] = "4:100:60:20:30/20:30|10:40/10:40>120"; //4:10:6:2:3/3:2|1:4/4:1>12
char buffer[1024];

int contGlobal = 0;

Serie series[TAM_MAX];


void setup() {
  Serial.begin(115200);
  esp.begin(115200);  
  pinMode(INTERRUPT, INPUT);
  fita.begin(); // This initializes the NeoPixel library.
  fita.show();  // Como nenhum pixel foi iniciado, esse comando apaga todos os leds

  //ligar rele para evitar queimar a fonte. Sugestoes...

  Serial.println(F("Iniciando a fita"));
  delay(3000);

  // variar luminosidade de todos os leds 0 - 255. So tem efeito dps que a funçao show eh chamada
  fita.setBrightness(10);

  Serial.print(F("Memoria antes do inicio do programa = "));
  Serial.println(freeMemory());

  estado = LEITURA; //ou verificar eeprom para ver se tem alguma serie guardada.

}


void loop() {

  // maquina de estados
  Serial.print(F("Memoria loop = "));
  Serial.println(freeMemory());

  if (estado == LEITURA) {
    Serial.println(F("Leitura"));
    //pega todo treino enviado pelo app. msg enviada no formato: repeticao:distancia:intervalo:trecho1:trecho2/tempo1:tempo2|trecho1:trecho2/tempo1:tempo2>descanso_serie; ...

    pega_treino();


  }
  else if (estado == TREINO) {
    Serial.println(F("Comeco Treino"));

    attachInterrupt(digitalPinToInterrupt(INTERRUPT), pause, LOW);
    treino();
    detachInterrupt(digitalPinToInterrupt(INTERRUPT));

    Serial.println(F("Fim Treino"));


  }
  else {
    Serial.println(F("Atribuicao"));
    // Estado de ATRIBUICAO: atribui a proxima serie à struct Serie
    pega_serie();
  }

}


//FUNCAO DE INTERRUPCAO

void pause() {
  //pinMode(2, INPUT);
  while (digitalRead(INTERRUPT) != 1) {
    //Serial.println("ISR");
  }
}



/********************

   Comunicacao e manipulacao de dados

 ********************/

bool enviaEstado() {

  esp.println("1"); //quer dizer que é o estado de leitura
  delay(200);
  unsigned long timeout = millis();

  while (esp.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(F(">>> Client Timeout !"));
      //envio do estado nao deu certo - servidor nao respondeu
      return 0;
    }
  }
  delay(500); //delay necessario para que cheguem todos os dados no buffer serial

  uint8_t cont = 0;
  char bufferAux[5];

  while (esp.available()) {
    bufferAux[cont] = (char)esp.read();
    cont++;
  }
  bufferAux[cont] = '\0';
  Serial.println(bufferAux);

  if (strcmp(bufferAux, "ok")) {
    //envio do estado nao deu certo
    return 0;
  }

  //envio do estado deu certo
  return 1;

}

void pega_treino() {

  while (!enviaEstado()) {
    /*Nao faz nada*/
  }
  Serial.println(F("Envio do estado de leitura realizado"));

  while (esp.available() == 0) {
    //enquanto nao houver dado enviado pelo app, ficar esperando
  }
  delay(500); //delay necessario para que cheguem todos os dados no buffer serial

  int cont = 0;

  while (esp.available()) {
    buffer[cont] = (char)esp.read();
    cont++;
  }

  buffer[cont] = '\0';

  //Debugg
  Serial.println(buffer);

  contGlobal = 0;

  estado = ATRIBUICAO;
}

int procuraChar() {

  int i = 0;
  char bufferAux[10]; //nao precisa ter mts caracteres, ja que ira armazenar inteiros temporariamente

  while (buffer[contGlobal] != ':' && buffer[contGlobal] != '\0' && buffer[contGlobal] != ';') {
    bufferAux[i] = buffer[contGlobal];
    contGlobal++;
    i++;
  }
  bufferAux[i] = '\0';

  if (buffer[contGlobal] != '\0') {
    contGlobal++; //incrementa para sair da condicao de stop
  }

  Serial.print(F("Memoria procuraChar = "));
  Serial.println(freeMemory());

  return atoi(bufferAux);
}

void procuraIdaVolta() {

  int i = 0;
  char bufferAux[10]; //nao precisa ter mts caracteres, ja que ira armazenar inteiros temporariamente
  bool sem_volta = 1; //flag para saber se o app mandou a string com as informacoes de ida e volta ou so de ida
  uint8_t contAux = 0;


  /*******
    ATRIBICAO DA IDA
  *******/


  while (buffer[contGlobal] != '/') {
    if (buffer[contGlobal] == ':') {
      bufferAux[i] = '\0';
      series[qtd_serie].ida.trecho[contAux] = atoi(bufferAux);
      contGlobal++;
      contAux++;
      i = 0;
    } else {
      bufferAux[i] = buffer[contGlobal];
      contGlobal++;
      i++;
    }
  }

  bufferAux[i] = '\0';
  series[qtd_serie].ida.trecho[contAux] = atoi(bufferAux);
  contGlobal++;
  i = 0;

  qtdIda = contAux + 1;
  contAux = 0;

  while (buffer[contGlobal] != '|' && buffer[contGlobal] != '>') {
    if (buffer[contGlobal] == ':') {
      bufferAux[i] = '\0';
      series[qtd_serie].ida.tempo[contAux] = atoi(bufferAux);
      contGlobal++;
      contAux++;
      i = 0;
    } else {
      bufferAux[i] = buffer[contGlobal];
      contGlobal++;
      i++;
    }
  }

  bufferAux[i] = '\0';
  series[qtd_serie].ida.tempo[contAux] = atoi(bufferAux);
  contGlobal++;
  i = 0;

  if (buffer[contGlobal - 1] == '|') {
    sem_volta = 0;
  }


  /*******
    ATRIBICAO DA VOLTA
  *******/

  contAux = 0;

  //app mandou a volta na string
  if (sem_volta == 0) {
    while (buffer[contGlobal] != '/') {
      if (buffer[contGlobal] == ':') {
        bufferAux[i] = '\0';
        series[qtd_serie].volta.trecho[contAux] = atoi(bufferAux);
        contGlobal++;
        contAux++;
        i = 0;
      } else {
        bufferAux[i] = buffer[contGlobal];
        contGlobal++;
        i++;
      }
    }

    bufferAux[i] = '\0';
    series[qtd_serie].volta.trecho[contAux] = atoi(bufferAux);
    contGlobal++;
    i = 0;

    qtdVolta = contAux + 1;
    contAux = 0;

    while (buffer[contGlobal] != '>') {
      if (buffer[contGlobal] == ':') {
        bufferAux[i] = '\0';
        series[qtd_serie].volta.tempo[contAux] = atoi(bufferAux);
        contGlobal++;
        contAux++;
        i = 0;
      } else {
        bufferAux[i] = buffer[contGlobal];
        contGlobal++;
        i++;
      }
    }

    bufferAux[i] = '\0';
    series[qtd_serie].volta.tempo[contAux] = atoi(bufferAux);
    contGlobal++;
    i = 0;

  }

  // app so mandou as idas. Isso significa que as idas devem ser atribuidas iguais as voltas
  else {
    for (int i = 0; i < qtdIda; i++) {
      series[qtd_serie].volta.trecho[i] = series[qtd_serie].ida.trecho[i];
      series[qtd_serie].volta.tempo[i] = series[qtd_serie].ida.tempo[i];
    }

    qtdVolta = qtdIda;

  }

  Serial.print(F("Memoria procuraIdaVolta = "));
  Serial.println(freeMemory());

}

void pega_serie() {


  series[qtd_serie].repeticao = procuraChar();
  series[qtd_serie].distancia = procuraChar();
  series[qtd_serie].descanso_distancia = procuraChar();

  //PEGA TRECHOS E TEMPOS
  procuraIdaVolta();

  series[qtd_serie].descanso_serie = procuraChar();


#ifdef DEBUG
  Serial.print(F("repeticao:"));
  Serial.println((series[qtd_serie].repeticao));
  Serial.print(F("distancia:"));
  Serial.println((series[qtd_serie].distancia));
  Serial.print(F("descanso_distancia:"));
  Serial.println((series[qtd_serie].descanso_distancia));
  Serial.print(F("descanso serie:"));
  Serial.println((series[qtd_serie].descanso_serie));

  Serial.println("Idas trechos");

  for (int i = 0; i < qtdIda; i++) {
    Serial.println(series[qtd_serie].ida.trecho[i]);
  }

  Serial.println("Idas tempos");

  for (int i = 0; i < qtdIda; i++) {
    Serial.println(series[qtd_serie].ida.tempo[i]);
  }

  Serial.println("Voltas trechos");
  for (int i = 0; i < qtdVolta; i++) {
    Serial.println(series[qtd_serie].volta.trecho[i]);
  }

  Serial.println("Voltas tempos");
  for (int i = 0; i < qtdVolta; i++) {
    Serial.println(series[qtd_serie].volta.tempo[i]);
  }
#endif

  estado = TREINO;

}


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

      NUM_PISCINAS = (series[i].distancia) / tamanho;
      for (int j = 0; j < NUM_PISCINAS; j++) {

        if (j % 2 == 0) {
          //manda as piscinas de ida
          velocidade_constante(series[i].ida.tempo, series[i].ida.trecho, 0);
        } else {
          //manda as piscinas de volta
          velocidade_constante(series[i].volta.tempo, series[i].volta.trecho, 1);
        }
        delay(500);//tirar delay???
      }
      //se for a ultima repeticao nao ha intervalo, ou seja, nao entra no if
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

    int tempo_aux = series[i].descanso_serie * 1000;
    if (tempo_aux >= 5000) {
      delay(tempo_aux - 5000);
    }

  }

  if (buffer[contGlobal] == '\0') {
    estado = LEITURA; //se acabar todas as series enviadas pelo app, esperar nova resposta
  }
  else {
    estado = ATRIBUICAO;
  }

  Serial.print(F("Memoria treino = "));
  Serial.println(freeMemory());
}


void velocidade_constante(int tempo[], int trecho[], bool flag) {

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
        pixel_color = fita.Color(0, 150, 0); //azul, caso lento
        break;

      case 1:
        pixel_color = fita.Color(150, 0, 0); //vermelho, caso rapido
        break;

      case 2:
        pixel_color = fita.Color(0, 0, 150); //verde, inicio da piscina
        break;
    }



    ledControl(delayval, pos_inicial, pos_final, pixel_color);

    Serial.print(F("Memoria velconstante = "));
    Serial.println(freeMemory());


  }
}
void acelerado(int tempo[], int trecho[], bool flag) {

  int pos_inicial, pos_final;
  uint8_t cor = 0;
  int t = 0;
  uint8_t sz;

  if (flag == 0) {
    //idas
    sz = qtdIda;
  }
  else {
    //voltas
    sz = qtdVolta;
  }

  for (int j = 1; j <= sz; j++) {

    Serial.print(F("Trecho: "));
    Serial.println(j);
    Serial.print(F("Tempo: "));
    Serial.println(tempo[j - 1]);

    int NRO_DELAY = ((trecho[j - 1] * 10) - 1);
    int delayvector[NRO_DELAY];
    int delayval = (tempo[j - 1] * 1000) / (trecho[j - 1] * 10); //conversao do tamanho para pixels (multiplicar por 10);
    int delayinit = delayval + ((NRO_DELAY) * aceleracao) / 2; //Formula para encontrar o primeiro valor de delay;
    int delayaux = delayinit;

    for (t = 0 ; t < NRO_DELAY ; t++) {
      delayvector[t] = delayaux;
      delayaux = delayaux + aceleracao;
    }

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
        pixel_color = fita.Color(0, 150, 0); //azul, caso lento
        break;

      case 1:
        pixel_color = fita.Color(150, 0, 0); //vermelho, caso rapido
        break;

      case 2:
        pixel_color = fita.Color(0, 0, 150); //verde, inicio da piscina
        break;
    }


    ledControlAcelerado(delayvector, pos_inicial, pos_final, pixel_color);

  }
}

int comparaCor(int tam_ante, int tempo_ante, int tam_atual, int tempo_atual) {

  float vel_atual, vel_ante;
  tempo_atual = (tempo_atual / 1000);
  tempo_ante = (tempo_ante / 1000);
  vel_atual = ((float)tam_atual / tempo_atual);
  vel_ante = ((float)tam_ante / tempo_ante);

  if ((vel_atual / vel_ante) > 1) {
    return 1;
  }
  else if ((vel_atual / vel_ante) < 1) //otimizar se preciso
    return 0;

  Serial.print(F("Memoria comparaCor = "));
  Serial.println(freeMemory());
}


void calculo_pos(int tam_trecho, int *pos_inicial, int *pos_final) {

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


      fita.setPixelColor(i, cor); //define a cor do pixel i


      if (i != 0) {

        fita.setPixelColor(i - 1, fita.Color(0, 0, 0));

        if (i == NUMPIXELS - 1) {
          direcao = 'v';
        }

      }

      fita.show(); // This sends the updated pixel color to the hardware.

      delay(delayval); // Delay for a period of time (in milliseconds).

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

      delay(delayval); // Delay for a period of time (in milliseconds).

    }
  }

  Serial.print(F("Memoria ledControl = "));
  Serial.println(freeMemory());
}
void ledControlAcelerado(int delayval[], int pos_inicial, int pos_final, uint32_t cor) {

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

      delay(delayval[i]); // Delay for a period of time (in milliseconds).

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

      delay(delayval[i]); // Delay for a period of time (in milliseconds).

    }
  }
}


//funcao que sinaliza o começo de uma piscina. Primeiro acende-se os primeiros 2m de led e vai diminuindo a quantidade de leds acesos
//de 0.5 em 0.5m a cada 1 segundo. No total são 5 segundos para o começo do treino.
void sinaliza_comeco() {
  if (direcao == 'i') {

    for (int i = 19; i >= 0; i--) {
      fita.setPixelColor(i, fita.Color(0, 0, 150));
      fita.show();
    }
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
      fita.setPixelColor(i, fita.Color(0, 0, 150));
      fita.show();
    }
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

  Serial.print(F("Memoria sinalizaComeco = "));
  Serial.println(freeMemory());

}
