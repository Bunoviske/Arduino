#include "FSM.h"

/**************************************
                CODIGO
**************************************/

/*
   A logica do treino é dividida em series e piscinas. Cada serie é definida por uma certa quantidade de repeticoes
   e cada piscina tem como caracteristica trechos determinados pelo usuario e o tempo que ele deve percorrer
   em cada um deles. Além disso, é definido um intervalo entre cada distancia e o descanso entre cada
   serie.

   -> tratar no app para que seja no maximo n trechos ---> atualmente sao 5. Tambem tratar tempo minimo de intervalo/descanso

   ->precisao de uma casa decimal para trechos e de ate 3 casas decimais para o tempo. ver isso no app. alem disso, soma dos trechos tem q dar o tamanho da piscina

*/

/*
 * IMPORTANTE: INFORMACOES DE MEMORIA
 * 
 * CODIGO UTILIZA 311 BYTES SEM BUFFER E SEM BIBLIOTECA DE CONTROLE DA FITA 
 * 
 */

//INSERIR ESTADO DE ACKNOWLEDGE DO ARDUINO PARA OS DADOS ENVIADOS PELO ESP!

void setup() {
  initFSM();
  Serial.println(freeMemory());
}

uint8_t state = LEITURA; //estado inicial

void loop() {
  state = changeState(state);
}


