#include "wireCom.h"

/******

  Comentarios sobre o codigo

 ******/

//nao vai ter mais a struct de treinos. Só haverá series, entao a divisão em varios treinos deverá ser feito apenas no aplicativo. Assim, o tempo
//entre um treino e outro nada mais é que o tempo entre uma série e outra no arduino

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
  estado = LEITURA;

  Serial.println(NUMPIXELS);
  Serial.println(F("Iniciando a fita"));
  Serial.print(F("Memoria antes do inicio do programa = "));
  Serial.println(freeMemory());
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

    //4:10:6:2:3/3:2|1:4/4:1>12
    //unsigned long aux = millis();
    treino();
    //Serial.println(millis() - aux);

  }
  else {
    Serial.println(F("Atribuicao"));
    //Estado de ATRIBUICAO: atribui a proxima serie à struct Serie
    pega_serie();
  }

}



