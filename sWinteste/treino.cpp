#include "treino.h"

//globais privadas
static int atual_pos = 0; //ultima posicao do led aceso
static int final_pos = 0; //posicao final de um trecho
static uint16_t qtdPiscinas = 0;
static uint16_t TIMER_COUNT = 0;
static uint8_t contTrecho = 0;
static int8_t offset = 1;
static uint8_t flagSinalizaComeco = 1;

volatile char FLAG_ISR = 'N'; //nao usar static aqui!!! Nao igualar FLAG_ISR = 0!

//essas duas variaveis podem ser 8 bits pois o pior caso eh o usuario pedir para o led esperar 17min antes de ir para prox posicao (considerando prescale 1024), ou
// o intervalo ser de 17min
static uint8_t qtdMaxInterrupts = 0;
static uint8_t qtdInterrupts = 0;

static CRGB fita[MAX_LEDS]; //modo normal ---->usar BRG. na memoria sao (MAX_LEDS*3) bytes IMPORTANTE: variaveis que acessam o vetor (index) precisam ser INT!!!!!!


unsigned long tempo = 0;

uint8_t treino() {

  if (sb::goSleep == 1) {
    setNextStateVariables(SLEEP);
    return SLEEP;
  }

  //SUB MAQUINA DE ESTADOS DE TREINO

  if (sb::subState == INIT_TREINO) return ledConfig();

  else if (sb::subState == PAUSE) return pause();

  else if (sb::subState == LOGICA) return logica();

  else if (sb::subState == TREINAR) return treinar();

  else if (sb::subState == INTERVALO) return intervalo();

  else if (sb::subState == SINALIZA_COMECO) return sinalizaComeco();

  else  { //STOP
    setNextSubStateVariables(STOP);
    setNextStateVariables(LEITURA);
    return LEITURA;
  }

}

static void setNextStateVariables(uint8_t nextState) {
  if (nextState == LEITURA) {
    sb::sendState = 1;
    clearAllLeds();
    digitalWrite(TRANSISTOR, HIGH);
  }
  else { //proximo estado: sleep
    sb::direcao = 'i';
    atual_pos = 0;
    flagSinalizaComeco = 1;
    clearAllLeds();
  }
}

void setNextSubStateVariables(uint8_t nextSubState) {
  if (nextSubState == LOGICA) {
    OCR1B = 65535;
    checkContTrechos(); //checa se o trecho terminou, alterando algumas variaveis para dar continuidade a serie na prox vez que voltar para esse sub estado
    sb::subState = LOGICA;
  }
  else if (nextSubState == TREINAR) {
    OCR1B = 65535;
    qtdInterrupts = 0;
    FLAG_ISR = 'N'; //redundante, porem necessario!!! (testes empiricos)
    sb::subState = TREINAR;
  }
  else if (nextSubState == PAUSE) {
    TIMER_COUNT = TCNT1; //variavel auxiliar recebe valor de contagem do timer, para saber onde parou!
    sb::goPause = 1; //garante que essa variavel eh um na primeira vez q entra no estado PAUSE, para garantir a recursao nele
    sb::subState = PAUSE;
  }
  else if (nextSubState == INTERVALO) {
    qtdInterrupts = 0;
    sb::startNow = 0; //garante q essa variavel nao esta alterada
    logicaIntervalo();

    //necessario fazer esse teste pois pode existir tempo de descanso = 0!!!
    if (OCR1B <= TCNT1) { //Nao precisa disso na parte do treino pois provavelmente nao tera um valor no comparador tao pequeno! VER DPS CASO DE PROBLEMA
      FLAG_ISR = 'B'; //nao tem que esperar interrupcao para terminar o estado INTERVALO. //caso OCR1B == 0 (sem descanso):
    }
    else
      FLAG_ISR = 'N'; //redundante, porem necessario!!! (testes empiricos)

    //Serial.println(FLAG_ISR);
    sb::subState = INTERVALO;
  }
  else if (nextSubState == SINALIZA_COMECO) {
    offset = 19; //variavel auxiliar!!!! para nao ter q criar outra variavel, usa-se offset para controlar o startSinalizaComeco!!!
    OCR1B = 15625; //1s
    TIMSK1 = ( 1 << OCIE1B); //habilita comparacao apenas em B
    FLAG_ISR = 'N'; //redundante, porem necessario!!! (testes empiricos)
    sb::subState = SINALIZA_COMECO;
  }
  else { //STOP
    flagSinalizaComeco = 1;
    sb::direcao = 'i';
    atual_pos = 0; //tres variaveis devem ser zeradas ("reset" do arduino)
    //clearAllLeds ja esta incluido na troca de estado para leitura!!!
    //nao precisa mudar subState pois ele ja esta atribuido
  }
}

/*********************
   FUNCOES DA SUB MAQUINA DE ESTADOS
 *********************/

uint8_t ledConfig() {

  //toda vez que ha um stop ou sleep, deve atribuir direcao = 'i' e atual_pos = 0
  //se nenhum desses dois estados acontecerem, o arduino ira guardar a ultima posicao do led para a prox serie que chegar

  //inicializa variaveis que podem ter sido alteradas no final do ultimo treino/serie
  sb::startNow = 0;
  sb::goPause = 0;

  final_pos = 0; //vai ser alterada antes de comecar o treino
  qtdPiscinas = 0;
  contTrecho = 0;
  qtdMaxInterrupts = 0;
  qtdInterrupts = 0;
  FLAG_ISR = 'N';

  clearAllLeds();
  digitalWrite(TRANSISTOR, LOW);
  digitalWrite(PIN_ESP, LOW);
  FastLED.addLeds<WS2811, PIN_WS2811, BRG>(fita, sb::NUMPIXELS); // tamanho da piscina definido pelo app
  FastLED.setBrightness(255);

  initTimer(); //essa funcao ja inicializa OCR1B = MAX
  //setNextSubStateVariables(LOGICA); //nao chamar essa funcao pois a primeira vez que entra na logica, checkContTrechos nao deve ser chamada
  sb::subState = LOGICA; //mudar apenas o subState
  Serial.println(freeMemory());
  return TREINO;
}


uint8_t pause() {

  //Serial.println(F("Pause!"));

  if (sb::goPause == 0) { //so entra quando for despausado
    Serial.println(F("Entrei no pause!"));
    Serial.println(freeMemory());
    if (qtdPiscinas == 0) { //chamar estado que sinaliza comeco
      qtdPiscinas++;
      digitalWrite(PIN_ESP, LOW);
      if (flagSinalizaComeco == 1 ) {
        setNextSubStateVariables(SINALIZA_COMECO);
        return TREINO;
      }
      //se entrou aqui é pq nao houve intervalo entre series!
      flagSinalizaComeco = 1; //a principio considera-se que a prox serie nao tem descanso_serie = 0. Caso tenha, logicaIntervalo ira modificar sinalizaComeco para 0!!!
      setFirstLed();//inicializa o timer agr (primeira serie de um treino)
      setNextSubStateVariables(TREINAR);
      return TREINO;
    }

    //se entrou aqui é pq houve pause durante a execucao de alguma parte do algoritmo. A parte superior indica apenas o pause esperando o play do usuario na primeira serie!

    TCNT1 = TIMER_COUNT; //contador recebe valor que estava contando na hr do pause.
    if (offset == 23) {
      sb::subState = INTERVALO; //volta de onde tava no estado INTERVALO, ou seja, nao deve-se chamar setNextSubStateVariables()
      TIMSK1 = ( 1 << OCIE1B); //habilita comparador B de novo!
      FLAG_ISR = 'N';
      Serial.println(FLAG_ISR);
    }
    else if (offset == 1 || offset == -1) {
      sb::subState = TREINAR; //se offset for igual a 1 ou -1, voltar para o estado TREINAR. volta de onde tava, ou seja, nao deve-se chamar setNextSubStateVariables()
      //Serial.println(F("Treinar"));
      //Serial.println(FLAG_ISR);
      //algum comparador deve ser habilitado!
      if (FLAG_ISR == 'A') {
        TIMSK1 = ( 1 << OCIE1A);
      }
      else TIMSK1 = ( 1 << OCIE1B);
      FLAG_ISR = 'N'; //zera variavel que foi usada para auxiliar essa logica!
      Serial.println(FLAG_ISR);
    }
    else { //caso em que houve despause durante sinalizaComeco!!! OFFSET NAO PODE SER NENHUM DOS VALORES ACIMA! Valores que ele assume durante sinComeco sao 19,14,9,4
      sb::subState = SINALIZA_COMECO;
      TIMSK1 = ( 1 << OCIE1B); //habilita comparador B de novo!
      FLAG_ISR = 'N';
      Serial.println(FLAG_ISR);
    }
  }
  return TREINO;
}


uint8_t logica() { //inicializa a logica do timer. so deve ser chamada quando percorrer ate o ultimo pixel do trecho

  Serial.print(F("LOGICA!  "));
  //Serial.println(atual_pos);

  int16_t pixelsTrecho = 0;

  //dividir logica entre piscinas de ida e piscinas de volta
  if (sb::direcao == 'i') { //ida

    offset = 1; //incremento positivo do led
    pixelsTrecho = sb::serieAtual.ida.trecho[contTrecho] * 10;
    setFinalPos(pixelsTrecho); //calcula-se posicao final do trecho

    float auxTime = (sb::serieAtual.ida.tempo[contTrecho]) / (pixelsTrecho - 1); //menos 1 pois sao numPixels - 1 intervalos de tempo
    //conversao do tamanho para pixels (multiplicar por 10). tempo em segs;

    if (auxTime * 15625 > 65535) { //15625 hz para prescale de 1024
      //se na menor escala a contagem nao caber, usar dois valores diferentes nos comparadores.
      OCR1A = (uint16_t)(round(auxTime * 15625) % 65535); //qtd de clocks da ultima interrupcao q der (apos n contagens maximas)
      qtdMaxInterrupts = (auxTime * 15625) / 65535;
    }
    else { //contagem cabe dentro de 15625hz
      OCR1A = (uint16_t)(round(auxTime * 15625)); //qtd de clocks
      qtdMaxInterrupts = 0;
    }
    //so compara a cor se nao for o primeiro trecho
    if (contTrecho > 0) setLedColor(sb::serieAtual.ida.trecho[contTrecho], sb::serieAtual.ida.tempo[contTrecho],
                                      sb::serieAtual.ida.trecho[contTrecho - 1], sb::serieAtual.ida.tempo[contTrecho - 1]);
    else {
      fita[atual_pos] = CRGB::Green;
      FastLED.show();
    }
  }
  else { //volta
    offset = -1; //incremento negativo do led
    pixelsTrecho = sb::serieAtual.volta.trecho[contTrecho] * 10;
    setFinalPos(-pixelsTrecho); //calcula-se posicao final do trecho. manda-se range de pixels negativo

    float auxTime = (sb::serieAtual.volta.tempo[contTrecho]) / (pixelsTrecho - 1); //menos 1 pois sao numPixels - 1 intervalos de tempo
    //conversao do tamanho para pixels (multiplicar por 10). tempo em segs;


    if (auxTime * 15625 > 65535) { //15625 hz para prescale de 1024
      //se na menor escala a contagem nao caber, usar dois valores diferentes nos comparadores.
      OCR1A = (uint16_t)(round(auxTime * 15625) % 65535); //qtd de clocks da ultima interrupcao q der (apos n contagens maximas)
      qtdMaxInterrupts = (auxTime * 15625) / 65535;
    }
    else { //contagem cabe dentro de 15625hz
      OCR1A = (uint16_t)(round(auxTime * 15625)); //qtd de clocks
      qtdMaxInterrupts = 0;
    }
    //so compara a cor se nao for o primeiro trecho
    if (contTrecho > 0) setLedColor(sb::serieAtual.volta.trecho[contTrecho], sb::serieAtual.volta.tempo[contTrecho],
                                      sb::serieAtual.volta.trecho[contTrecho - 1], sb::serieAtual.volta.tempo[contTrecho - 1]);
    else {
      fita[atual_pos] = CRGB::Green;
      FastLED.show();
    }
  }

  Serial.println(OCR1A);
  Serial.println(freeMemory());
  //Serial.println(OCR1B);
  //Serial.println(qtdMaxInterrupts);

  if (qtdPiscinas == 0) { //se for a primeira vez que entrou no estado de logica, inicializa sinalizaComeco e o prox estado eh pause automaticamente (ver prox linhas)
    if (flagSinalizaComeco == 1 ) setSinalizaComeco();
    digitalWrite(PIN_ESP, HIGH); //logica entre arduino e esp, para o ultimo saber se pode enviar dado por I2C. Nesse caso, se nao for a primeira serie,
    //o esp vai saber que pode despausar, sem esperar acao do usuario. Lembrete: usuario so da play se for comeco de treino
    setNextSubStateVariables(PAUSE);
    return TREINO;
  }
  //inicio de uma repeticao (q nao seja a primeira): verifica se eh o inicio de uma repeticao. Deve-se acender o primeiro led logo apos sinaliza comeco!!!!
  else if (checkIntervalRep() == 1) { //deve-se chamar sinalizaComeco entre repeticoes (SEMPRE)
    setSinalizaComeco();
    setNextSubStateVariables(SINALIZA_COMECO);
    return TREINO;
  }
  else { //inicio de um trecho que nao seja uma das opcoes acima! necessario apenas habilitar os comparadores.
    setComp();
    setNextSubStateVariables(TREINAR);
    return TREINO;
  }
}

void logicaIntervalo() { //nao eh um sub estado mas esta muito atrelado ao estado INTERVALO

  Serial.println(F("LOGICA INTERVALO"));
  Serial.println(freeMemory());

  if (qtdPiscinas == sb::serieAtual.repeticao * (sb::serieAtual.distancia / (sb::NUMPIXELS / 10))) { //se a qtd de piscinas na serie atingir o maximo, termina a serie
    //delay final de serie. para esse delay, faz sentido o usuario definir delay = 0, ja que ele pode querer fazer um treino progressivo (cada serie que ele manda o tempo muda)
    float auxTime = (sb::serieAtual.descanso_serie - 5.); //garantir no app que esse tempo seja zero ou maior igual a 5 se for delay entre series!!!!
    //alem disso, mais importante ainda, DEFINIR NO APP QUE O DELAY ENTRE TREINOS DEVE SER MAIOR QUE 5, senao a primeira serie do treino nao vai ter sinalizaComeco!!!  mudar isso dps caso precise

    if (auxTime < 0) { //caso em que nao deve haver tempo de espera entre as series e nem sinalizar o comeco
      OCR1B = 0;
      qtdMaxInterrupts = 1;
      flagSinalizaComeco = 0; //a primeira repeticao da proxima serie que o arduino receber estara indicada para nao chamar sinalizaComeco!!!
    }
    else if (auxTime * 15625 > 65535) {
      OCR1A = (uint16_t)(round(auxTime * 15625) % 65535); //COMPA esta armazenando temporariamente esse valor. esse comparador nao eh usado nesse sub estado
      qtdMaxInterrupts = (auxTime * 15625 / 65535) + 1;//soma-se um pois deve entrar mais uma vez na funcao de interrupcao, na comp referente ao valor armazenado em A
    }
    else {
      OCR1B = (uint16_t)(round(auxTime * 15625));
      qtdMaxInterrupts = 1;
    }
  }
  else { //se nao terminou a serie, delay entre repeticoes. Nao faz sentido nao ter delay aqui. Se o usuario quiser a serie continua, colocar uma repeticao com distancia grande!
    float auxTime = (sb::serieAtual.descanso_distancia - 5.); //garantir no app que o intervalo eh maior >= 5!!!! IMPORTANTE!!!!!

    if (auxTime * 15625 > 65535) {
      OCR1A = (uint16_t)(round(auxTime * 15625) % 65535); //COMPA esta armazenando temporariamente esse valor. esse comparador nao eh usado nesse sub estado
      qtdMaxInterrupts = (auxTime * 15625 / 65535) + 1; //soma-se um pois deve entrar mais uma vez na funcao de interrupcao, na comp referente ao valor armazenado em A
    }
    else {
      OCR1B = (uint16_t)(round(auxTime * 15625));
      qtdMaxInterrupts = 1;
    }
  }

  //Serial.println(FLAG_ISR);
  TIMSK1 = ( 1 << OCIE1B); //habilita comparacao apenas em B
  //Serial.println(FLAG_ISR);

}

uint8_t intervalo() {

  if (FLAG_ISR == 'B') {
    //Serial.println(FLAG_ISR);
    FLAG_ISR = 'N';
    qtdInterrupts++;

    if (qtdInterrupts == qtdMaxInterrupts || sb::startNow == 1) { //acabou o intervalo.
      /*Se startNow for atribuido pelo usuario, cancela-se o tempo de espera. Essa funcao ta habilitada para cancelar intervalos tanto de repeticoes quanto de series,
        entao eh importante que no app essa funcao so seja habilitada entre series!!!!!*/
      sb::startNow = 0;
      qtdInterrupts = qtdMaxInterrupts; //caso entre na funcao pelo startNow e um pause tenha ocorrido durante a execucao dessa condicao

      TIMSK1 &= ~(1 << OCIE1B); //desabilita ISR

      //OCR1B voltar a ser maximo em qualquer estado seguinte que for determinado
      if (qtdPiscinas == sb::serieAtual.repeticao * (sb::serieAtual.distancia / (sb::NUMPIXELS / 10))) { //se acabou a serie, retornar para leitura

        //como acabou a serie, é preciso mudar a direcao da ultima piscina que foi realizada!
        if (sb::direcao == 'i') sb::direcao = 'v';
        else sb::direcao = 'i';

        Serial.println(F("Fim serie!!!"));

        setNextStateVariables(LEITURA);
        return LEITURA; //se a serie tiver sido executada
      }
      else { //retornar para logica
        Serial.println(F("Fim repeticao!!!"));
        setNextSubStateVariables(LOGICA);
      }
    }
    else {
      //if (qtdMaxInterrupts > 1) { //condicao redundante!
      if (qtdInterrupts == qtdMaxInterrupts - 1) {
        OCR1B = OCR1A; //muda o valor para a ultima interrupcao
      }
      // }
    }
  }

  //IMPORTANTE: else if igual ao sub estado TREINAR!!!
  else if (sb::goPause == 1) { //testa se entrou no estado pause
    TIMSK1 &= ~(1 << OCIE1B); //desabilita interrupcao
    FLAG_ISR = 'N';
    setNextSubStateVariables(PAUSE);

    /*Para indicar ao estado PAUSE que ele deve retornar o estado INTERVALO, usa-se a variavel offset como auxiliar,
      ja que ela nao esta sendo usada no momento e sera alterada para o valor correto antes de ser utilizada!!!!*/
    offset = 23;
  }
  return TREINO;
}
//estado no qual fica rodando a serie
uint8_t treinar() {
  //if (FLAG_ISR == 0) Serial.println(FLAG_ISR);
  /*caso generico, ou seja, independente se eh usado comparador B que auxilia a contagem de tempos grandes, a funcao escolhe qual dos dois ...
    ... comparadores esta ativo no momento*/

  if (FLAG_ISR == 'A') { //comp A
    //Serial.println(FLAG_ISR);
    FLAG_ISR = 'N';

    //pixel na posicao atual ja tem a cor que ele deve acender. Basta propaga-la para os pixels posteriores
    if (atual_pos + offset <= sb::NUMPIXELS - 1 && atual_pos + offset >= 0)   fita[atual_pos + offset] = fita[atual_pos]; //acende de dois em dois pixels
    if (atual_pos - offset <= sb::NUMPIXELS - 1 && atual_pos - offset >= 0)   fita[atual_pos - offset] = CRGB::Black; //apaga pixel anterior
    atual_pos += offset;

    if (atual_pos == final_pos) {
      TIMSK1 &= ~(1 << OCIE1A) & ~(1 << OCIE1B); //desabilita interrupcao

      Serial.println(millis()-tempo);

      if (atual_pos - offset <= sb::NUMPIXELS - 1 && atual_pos - offset >= 0)   fita[atual_pos - offset] = CRGB::Black; //apaga pixel anterior.
      //esta aqui apenas para detalhe de visualizacao

      if (checkIntervalEnd() == 1) {    //verifica se ja terminou uma repeticao
        setNextSubStateVariables(INTERVALO);
      }
      else { //senao, eh pq terminou apenas uma piscina ou trecho
        setNextSubStateVariables(LOGICA);
      }
    }
    else if (qtdMaxInterrupts > 0) { //caso em que o comparador B é usado. Deve-se desabilitar COMP em A!!!
      TIMSK1 = ( 1 << OCIE1B); //habilita comp apenas em B.
    }

    FastLED.show();
  }

  else if (FLAG_ISR == 'B') { //comp B
    //Serial.println(FLAG_ISR);
    FLAG_ISR = 'N';
    qtdInterrupts++;
    if (qtdInterrupts == qtdMaxInterrupts) {
      TIMSK1 = ( 1 << OCIE1A); //habilita comp apenas em A.
      qtdInterrupts = 0;
    }
  }

  //IMPORTANTE: colocar um else if para que nao seja possivel haver conflito nas condicoes de cima e no pause??
  //Ex.: se o treino acabar e entrar no pause, apos a volta do ultimo, o contador atual_pos vai estourar sua posicao
  else if (sb::goPause == 1) { //testa se entrou no estado pause

    //como somente um dos comparadores esta habilitado por vez, eh possivel realizar a logica abaixo!
    if (bitRead(TIMSK1, OCIE1A)) FLAG_ISR = 'A'; //se o valor do comparador A estiver habilitado para comparacao no momento do pause, deve-se habilita-lo novamente dps
    else FLAG_ISR = 'B'; //comparador B estava habilitado
    //FLAG_ISR sera zerado no estado pause!

    //para saber qual comparador pause deve habilitar de novo, armazena-se essa informacao em FLAG_ISR e dps zera essa variavel no estado PAUSE!
    TIMSK1 &= ~(1 << OCIE1A) & ~(1 << OCIE1B); //desabilita interrupcao

    //Serial.println(FLAG_ISR);
    setNextSubStateVariables(PAUSE);
  }

  //senao, mantem no estado que o treino esta acontecendo

  return TREINO;
}

uint8_t sinalizaComeco() {

  if (FLAG_ISR == 'B') {
    //Serial.println(F("Sin"));
    FLAG_ISR = 'N';

    //acabou sinalizaComeco!
    if (offset == -2) {
      TIMSK1 &= ~(1 << OCIE1B); //desabilita interrupcao
      setFirstLed();//inicializa o timer agr (comeco da repeticao)
      //volta valor do offset para o estado TREINAR
      if (sb::direcao == 'i') offset = 1;
      else offset = -1;
      setNextSubStateVariables(TREINAR);
    }
    else {
      int8_t aux = offset - 5;
      if (sb::direcao == 'i') {
        for (; offset > aux; offset--) {
          fita[offset] = CRGB::Black;
        }
      }
      else {
        for (; offset > aux; offset--) {
          fita[sb::NUMPIXELS - offset - 1] = CRGB::Black;
        }
      }
      FastLED.show();

      //quer dizer que acabou sinalizaComeco, mas eh necessario esperar mais 1 segundo para comecar o treino!
      if (offset == -1) offset--; //para evitar que tenha um pause e o offset valha -1. Iria dar problema no estado pause, na hr de decidir para qual estado retornar!!!
    }
  }
  else if (sb::goPause == 1) { //testa se entrou no estado pause
    TIMSK1 &= ~(1 << OCIE1B); //desabilita interrupcao
    FLAG_ISR = 'N';
    setNextSubStateVariables(PAUSE);
  }
  return TREINO;
}


/*********************
   FUNCOES AUXILIARES
 *********************/

//compara velocidade dos trechos e define a cor!!!!
void setLedColor(float tam_atual, float tempo_atual, float tam_ante, float tempo_ante) {
  float vel_atual, vel_ante;
  vel_atual = tam_atual / tempo_atual;
  vel_ante = tam_ante / tempo_ante;

  if (vel_atual > vel_ante)  { //caso rapido (vel atual maior que anterior)
    fita[atual_pos] = CRGB::Red;
  }
  else if (vel_atual < vel_ante) { //caso lento (vel atual menor que anterior)
    fita[atual_pos] = CRGB::Blue;
  }
  else {
    fita[atual_pos] = CRGB::Green; //mesma velocidade
  }
  FastLED.show(); //na transicao de cores, smp eh necessario acenda-las aqui! Quando for o comeco de uma repeticao ou serie, usa-se setFirstLed!!!
}

//verifica se terminou uma repeticao no estado treinar
uint8_t checkIntervalEnd() {

  //so retorna que acabaram todos os trechos da piscina se o contTrechos tiver atingido o maximo!
  if (sb::direcao == 'i') {
    if (contTrecho == sb::qtdIda - 1) {
      if (qtdPiscinas % (sb::serieAtual.distancia / (sb::NUMPIXELS / 10)) == 0) { //se o resto for zero, quer dizer que terminou uma repeticao da serie
        return 1;
      }
    }
  }
  else {
    if (contTrecho == sb::qtdVolta - 1) {
      if (qtdPiscinas % (sb::serieAtual.distancia / (sb::NUMPIXELS / 10)) == 0) { //se o resto for zero, quer dizer que terminou uma repeticao da serie
        return 1;
      }
    }
  }

  //OBS.: caso em que divide por 1 e o resto da sempre zero nao eh um problema para essa funcao, ja que retorna 1 caso o resto seja igual a zero

  return 0;
}

//verifica se eh o inicio de uma repeticao (que nao seja a primeira) no estado logica. PROBLEMA QUANDO DIVIDE POR 1: RESTO SEMPRE ZERO
uint8_t checkIntervalRep() {

  //como essa funcao eh chamada na logica, pode ser que esteja sendo realizado a logica do segundo trecho da primeira piscina. Se nao colocar a condicao
  //contTrechos == 0, essa funcao retornaria 1
  if (contTrecho == 0) {

    //sempre que esse valor der 1 (uma piscina para completar a repeticao), quer dizer que essa funcao sempre deve retornar 1, ou seja,
    //smp que contTrechos for zero quer dizer que eh o inicio de uma nova repeticao!!!
    if ((sb::serieAtual.distancia / (sb::NUMPIXELS / 10)) == 1) {
      return 1;
    }

    if (qtdPiscinas % (sb::serieAtual.distancia / (sb::NUMPIXELS / 10)) == 1) { //se o resto for um, quer dizer que eh o inicio de uma repeticao da serie
      return 1;
    }
  }
  return 0;
}

void setFirstLed() { //na primeira serie e apos os delays, eh necessario chamar essa funcao, pois o primeiro led nao precisa esperar para ser aceso.
  //chamado toda vez que inicia uma repeticao tbm

  //Serial.println(F("setFirst"));

  fita[atual_pos] = CRGB::Green; //o primeiro pixel (posicao 0 ou NUMPIXELS - 1) smp deve receber verde
  FastLED.show();
  TCNT1 = 0;
  tempo = millis();
  setComp();
  
}

void setComp() {

  //habilitacao dos comparadores corretos! Apenas um é habilitado por vez!
  //Serial.println(FLAG_ISR);
  if (qtdMaxInterrupts > 0)
    TIMSK1 = ( 1 << OCIE1B); //com uso do comp B. Comeca habilitando-o apenas
  else
    TIMSK1 = ( 1 << OCIE1A); //sem uso do comp B. Habilita apenas o A

  //Serial.println(FLAG_ISR);
}

void setFinalPos(int16_t pixelsRange) {
  //smp eh considerado que a posicao atual sera a primeira a acender o led
  final_pos = atual_pos + pixelsRange - offset; //offset eh positivo na ida e negativo na volta
}

//essa funcao altera variaveis que irao influenciar a prox vez que o sub estado logica for chamado. Por isso tem que chamar checkInterval antes dela!!!
//so deve ser chamada dps que o estado logica tiver sido executado! por isso nao chama-la apos led_config
void checkContTrechos() {

  if (sb::direcao == 'i') {
    if (contTrecho == sb::qtdIda - 1) { //terminou uma piscina
      contTrecho = 0;
      sb::direcao = 'v'; //troca a direcao e zera a contagem dos trechos, alem de incrementar a qtd de piscinas percorridas
      qtdPiscinas++;
    }
    else {
      contTrecho++; //se nao tiver acabado a piscina, incrementa para o proximo trecho
    }
  }
  else {
    if (contTrecho == sb::qtdVolta - 1) { //terminou uma piscina
      contTrecho = 0;
      sb::direcao = 'i'; //troca a direcao e zera a contagem dos trechos, alem de incrementar a qtd de piscinas percorridas
      qtdPiscinas++;
    }
    else {
      contTrecho++;
    }
  }
}

void initTimer() {
  cli();//stop interrupts

  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B

  //TCNT1  = 0;//initialize counter value to 0
  //q hrs zerar o contador do timer??? RESPOSTA: apos sinaliza comeco

  //1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10); //15.625Khz

  OCR1B = 65535; //max valor possivel de contagem. esse comparador vai auxiliar o timer a contar mais que seu maximo
  //OCR1A SERA DEFINIDO NA LOGICA

  TIMSK1 &= ~(1 << OCIE1A) & ~(1 << OCIE1B); //comparadores comecam desabilitados.

  sei();//allow interrupts

}

/*********************
   FUNCOES DE MANIPULACAO DIRETA DA FITA DE LED
 *********************/

void setSinalizaComeco() {

  if (sb::direcao == 'i') {
    for (int i = 19; i >= 0; i--) {
      fita[i] = CRGB::Green;
    }
  }
  else {
    for (int  i = (sb::NUMPIXELS - 20); i <= sb::NUMPIXELS - 1; i++) {
      fita[i] = CRGB::Green;
    }
  }
  FastLED.show();

}

void clearAllLeds() {
  digitalWrite(TRANSISTOR, LOW);
  FastLED.addLeds<WS2811, PIN_WS2811, BRG>(fita, MAX_LEDS); //todos os 250 leds (maximo possivel)
  FastLED.clear();
  FastLED.show();
  delay(100);
  digitalWrite(TRANSISTOR, HIGH);
}

/*********************
   FUNCOES DE INTERRUPCAO
 *********************/

ISR(TIMER1_COMPA_vect) {
  TCNT1  = 0;
  FLAG_ISR = 'A';
}

ISR(TIMER1_COMPB_vect) { //usado tanto para auxiliar o treino (controle dos leds) quanto no estado de intervalo
  TCNT1  = 0;
  FLAG_ISR = 'B';
}


