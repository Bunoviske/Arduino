#include "treino.h"
#include "wireCom.h"

/******

  Comentarios sobre o codigo

 ******/

//nao vai ter mais a struct de treinos. Só haverá series, entao a divisão em varios treinos deverá ser feito apenas no aplicativo. Assim, o tempo
//entre um treino e outro nada mais é que o tempo entre uma série e outra no arduino

//lista ou vetor estatico??

//testar progressivo e regressivo

//tratar no app para que seja no maximo n trechos ---> atualmente sao 5. Tambem tratar tempo minimo de intervalo/descanso

//precisao de uma casa decimal para trechos e de ate 3 casas decimais para o tempo. ver isso no app. alem disso, soma dos trechos tem q dar o tamanho da piscina

/**************************************
                CODIGO
**************************************/

/*
   A logica do treino é dividida em series e piscinas. Cada serie é definida por uma certa quantidade de repeticoes
   e cada piscina tem como caracteristica trechos determinados pelo usuario e o tempo que ele deve percorrer
   em cada um deles. Além disso, é definido um intervalo entre cada distancia e o descanso entre cada
   serie.

*/

void setup() {

  configWire();
  initialConfig();

  readEEPROM();
  NUMPIXELS = tamPiscina * 10;
  Serial.println(NUMPIXELS);
  
  FastLED.addLeds<WS2811, PIN, BGR>(fita, NUMPIXELS); //800KHZ
  FastLED.setBrightness(10);


  Serial.println(F("Iniciando a fita"));
  Serial.print(F("Memoria antes do inicio do programa = "));
  Serial.println(freeMemory());

  estado = LEITURA;
}


void loop() {

  // maquina de estados
  Serial.print(F("Memoria loop = "));
  Serial.println(freeMemory());

  if (estado == LEITURA) {
    Serial.println(F("Leitura"));

    //pega todo treino enviado pelo app. msg enviada no formato:
    //repeticao:distancia:intervalo:trecho1:trecho2/tempo1:tempo2|trecho1:trecho2/tempo1:tempo2>descanso_serie; ...

    digitalWrite(TRANSISTOR, LOW);

    pega_treino();

  }
  else if (estado == TREINO) {
    Serial.println(F("Treino"));

    FastLED.clear();
    FastLED.show();
    delay(10);
    digitalWrite(TRANSISTOR, HIGH);

    //4:10:6:2:3/3:2|1:4/4:1>12
    treino();

  }
  else {
    Serial.println(F("Atribuicao"));
    // Estado de ATRIBUICAO: atribui a proxima serie à struct Serie
    pega_serie();
  }

}



