#include <avr/pgmspace.h>
#include <MemoryFree.h>
#include <SoftwareSerial.h>

/*
   LINK PARA INSTALACAO DA BIBLIOTECA
   https://github.com/adafruit/Adafruit_NeoPixel
*/
#include <Adafruit_NeoPixel.h>


/*
   LINK PARA INSTALACAO DA BIBLIOTECA
   https://github.com/zacsketches/Arduino_Vector
*/
#include<Vector.h>   //Mudar o arquivo .h da blibioteca na pasta "libraries": botar letra maiuscula em arduino.h


/**************************************

   DEFINES

**************************************/

#define LEITURA        0
#define TREINO         1
#define ATRIBUICAO     2

#define PIN            6

//Define o tamanho da fita em m
#define tamanho        5

//Tamanho dos vetores de series
#define TAM_MAX        1 //otimizar tamanho das variaveis para caber mais series e trocar o vetor dinamico da struct "Piscina" caso precise
//arduino mega aguenta TAM_MAX = 100 ocupando 46% de variavel global



/*
   A logica do treino é dividida em series e piscinas. Cada serie é define por uma certa quantidade de repeticoes
   e cada piscina tem como caracteristica trechos determinados pelo usuario e o tempo que ele deve percorrer
   em cada um deles. Além disso, é definido um intervalo entre cada distancia e o descanso entre cada
   serie.

*/

typedef struct {

  Vector<int> trecho; //armazenar os tempos em segundos
  Vector<int> tempo;

} Piscina;

typedef struct {

  Piscina ida;
  Piscina volta;
  int repeticao;
  int distancia;
  int descanso_serie;  //armazenar os tempos em segundos
  int descanso_distancia;


} Serie;

//fazer leitura da entrada de dados do usuario

//comunicacao?

//quando a distancia da serie for igual ao tamanho da piscina, a Piscina de ida e volta devem receber os mesmos dados ?????????????????????????? pensar!

//ver se separa em idas e voltas mesmo que a distancia seja o tamanho da piscina

//nao vai ter mais a struct de treinos. Só haverá series, entao a divisão em varios treinos deverá ser feito apenas no aplicativo. Assim, o tempo
//entre um treino e outro nada mais é que o tempo entre uma série e outra no arduino

//lista ou vetor estatico??

//progressivo e regressivo??

/**************************************
                CODIGO
**************************************/

SoftwareSerial esp(2, 3);

const PROGMEM int NUMPIXELS = tamanho * 10; //está sendo considerado uma fita que tem 30 leds/m e que é alimentada por 12V (controle de 3 leds por vez)

const PROGMEM Adafruit_NeoPixel fita = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);

//qtd de series
uint8_t qtd_serie = 0;

char direcao = 'i'; //variavel que determina se é ida ou volta ------> 'i' = ida e 'v' = volta

uint16_t ultima_pos = 0; //variavel que auxilia o controle dos trechos de cada piscina

uint8_t estado;

char buffer[500];
//String buffer;

int contGlobal = 0;

Serie series[TAM_MAX];


void setup() {
  Serial.begin(9600);
  esp.begin(9600);

  fita.begin(); // This initializes the NeoPixel library.
  fita.show();  // Como nenhum pixel foi iniciado, esse comando apaga todos os leds
  Serial.println(F("Iniciando a fita"));
  delay(3000);

  // variar luminosidade de todos os leds 0 - 255. So tem efeito dps que a funçao show eh chamada
  fita.setBrightness(10);

  Serial.print(F("Memoria antes da entrada dos dados = "));
  Serial.println(freeMemory());

  pega_series();

  Serial.print(F("Memoria dps da entrada dos dados = "));
  Serial.println(freeMemory());


  Serial.println(F("Termino da configuracao das series"));

  estado = LEITURA; //ou verificar eeprom para ver se tem alguma serie guardada.


}

void loop() {

  // maquina de estados

  switch (estado) {

    case LEITURA:
      //pega todo treino enviado pelo app. msg enviada no formato: repeticao:distancia:intervalo:trecho1:trecho2/tempo1:tempo2|trecho1/tempo1:descanso_serie; ...
      pega_treino();

    case TREINO:

      //habilitar interrupcao
      treino();
      //desabilitar interrupcao

    case ATRIBUICAO:

      //atribui a proxima serie à struct Serie
      pega_serie();

  }

}

void pega_treino() {

  while (esp.available() == 0) {
    //enquanto nao houver dado enviado pelo app, ficar esperando
  }
  int cont = 0;
  while (esp.available()) {
    buffer[cont] = (char)Serial.read();
    cont++;

    /*
       se buffer for string, fazer buffer = Serial.readString();

    */
  }
  buffer[cont] = '\0';

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

  if (buffer[contGlobal] != '\0') {
    contGlobal++; //incrementa para sair da condicao de stop
  }

  return atoi(bufferAux);
}

void procuraIdaVolta() {

  int i = 0;
  char bufferAux[10]; //nao precisa ter mts caracteres, ja que ira armazenar inteiros temporariamente
  bool flag = 0; //flag = 0 significa que é trecho e flag = 1 significa que é tempo
  bool stop = 0;

  while (stop == 0) {

    i = 0;
    while (buffer[contGlobal] != '/' && buffer[contGlobal] != ':' && buffer[contGlobal] != '|') {
      bufferAux[i] = buffer[contGlobal];
      contGlobal++;
      i++;
    }
    contGlobal++; //incrementa para sair da condicao de stop

    if (buffer[contGlobal] != '/'){
      //incrementa trecho
      
    }
  }

}

void pega_serie() {



  series[qtd_serie].repeticao = procuraChar();
  series[qtd_serie].distancia = procuraChar();
  series[qtd_serie].descanso_distancia = procuraChar();

  //PEGA TRECHOS E TEMPOS
  procuraIdaVolta();

  series[qtd_serie].descanso_serie = procuraChar();



}

void treino() {

  int NUM_PISCINAS;
  for (int i = 0; i < qtd_serie; i++) {
    sinaliza_comeco();
    for (int k = 0; k < series[i].repeticao; k++) {

      Serial.print(F("*****************REPETICAO****************    "));
      Serial.println(k + 1);

      NUM_PISCINAS = (series[i].distancia) / tamanho;
      for (int j = 0; j < NUM_PISCINAS; j++) {

        if (j % 2 == 0) {
          //manda as piscinas de ida
          velocidade_constante(series[i].ida.tempo, series[i].ida.trecho);
        } else {
          //manda as piscinas de volta
          velocidade_constante(series[i].volta.tempo, series[i].volta.trecho);
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
}


void velocidade_constante(Vector<int>& tempo, Vector<int>& trecho) {

  int pos_inicial, pos_final;
  int cor = 0;


  for (int j = 1; j <= tempo.size(); j++) {

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



    led_control(delayval, pos_inicial, pos_final, pixel_color);


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

}





//funcao que controla a ida e volta dos leds. A flag define se controla a ida ou a volta
void led_control(int delayval, int pos_inicial, int pos_final, uint32_t cor) {

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

}





/*****************


  INTERFACE COM O USUÁRIO


*****************/



int pega_series() {
  bool flag = 0;

  while (flag == 0) {

    if (qtd_serie != TAM_MAX) {

      Serial.println(F("Vc deseja fazer uma serie? (s/n)"));
      while (Serial.available() == 0) {
      }
      char resposta = (char)Serial.read();

      if (resposta == 's' || resposta == 'S')
      {
        pega_dados();
        qtd_serie++;

      }
      else if (resposta == 'n' || resposta == 'N') {
        flag = 1;

      } else {
        Serial.println(F("Erro! Digite (s/n)"));

      }
    }
    else {
      Serial.println(F("Quantidade maxima de series atingida"));
      flag = 1;
    }
  }

}


void pega_dados() {


  String resposta; //variavel local
  //resposta.reserve(20); //evita heap fragmentation

  Serial.println(F("Serie no formato: repeticoes x distancia; intervalo. Exemplo:  6 x 100m; 30s"));
  Serial.println(F("Informe a quantidade de repeticoes:"));
  while (Serial.available() == 0) {

  }
  resposta = Serial.readString();
  series[qtd_serie].repeticao = resposta.toInt();

  Serial.println(F("Informe a distancia em m: "));
  while (Serial.available() == 0) {

  }
  resposta = Serial.readString();
  series[qtd_serie].distancia = resposta.toInt();

  if (resposta.toInt() == tamanho) {
    Serial.println(F("Como foi selecionado apenas uma piscina, insira a volta como sendo igual a ida (esse tratamento sera feito no app)"));
  }

  Serial.println(F("Informe o intervalo em segs:"));
  while (Serial.available() == 0) {

  }
  resposta = Serial.readString();
  series[qtd_serie].descanso_distancia = resposta.toInt();

  Serial.println(F("Em quantos trechos deseja dividir as idas (se nao quiser dividir em varios trechos, insira um trecho):"));
  while (Serial.available() == 0) {

  }
  resposta = Serial.readString();
  int trechos = resposta.toInt();

  for (int i = 0; i < trechos; i++) {
    Serial.print(F("Informe o tamanho do trecho "));
    Serial.println(i + 1);
    while (Serial.available() == 0) {
    }
    resposta = Serial.readString();
    series[qtd_serie].ida.trecho.push_back(resposta.toInt());


    Serial.print(F("Informe o tempo para percorrer o trecho "));
    Serial.println(i + 1);
    while (Serial.available() == 0) {
    }
    resposta = Serial.readString();
    series[qtd_serie].ida.tempo.push_back(resposta.toInt());
  }

  Serial.println(F("Em quantos trechos deseja dividir as voltas (se nao quiser dividir em varios trechos, insira um trecho):"));
  while (Serial.available() == 0) {

  }
  resposta = Serial.readString();
  trechos = resposta.toInt();

  for (int i = 0; i < trechos; i++) {
    Serial.print(F("Informe o tamanho do trecho "));
    Serial.println(i + 1);
    while (Serial.available() == 0) {
    }
    resposta = Serial.readString();
    series[qtd_serie].volta.trecho.push_back(resposta.toInt());

    Serial.print(F("Informe o tempo para percorrer o trecho "));
    Serial.println(i + 1);
    while (Serial.available() == 0) {
    }
    resposta = Serial.readString();
    series[qtd_serie].volta.tempo.push_back(resposta.toInt());
  }

  Serial.println(F("Informe o intervalo entre essa serie e a proxima em segs. (se nao houver proxima serie, o app ira preencher esse valor como zero)"));
  while (Serial.available() == 0) {
  }
  resposta = Serial.readString();
  series[qtd_serie].descanso_serie = resposta.toInt();

}



