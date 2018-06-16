#include "treino.h"

//globais privadas
static int final_pos = 0; //posicao final de um trecho
static uint16_t qtdIdas = 0;
static uint16_t TIMER_COUNT = 0;
static int8_t offset = 1;

volatile char FLAG_ISR = 'N'; //nao usar static aqui!!! Nao igualar FLAG_ISR = 0!

//essas duas variaveis podem ser 8 bits pois o pior caso eh o usuario pedir para o led esperar 17min antes de ir para prox posicao (considerando prescale 1024), ou
// o intervalo ser de 17min
static uint16_t qtdMaxInterrupts = 0;
static uint16_t qtdInterrupts = 0;

unsigned long tempo = 0;

uint8_t treino() {


  //SUB MAQUINA DE ESTADOS DE TREINO

  if (gb::subState == INIT_TREINO) return ledConfig();

  else if (gb::subState == PAUSE) return pause();

  else if (gb::subState == LOGICA) return logica();

  else if (gb::subState == TREINAR) return treinar();

  else if (gb::subState == INTERVALO) return intervalo();

  else if (gb::subState == SINALIZA_COMECO) return sinalizaComeco();

  else  { //STOP
    setNextSubStateVariables(STOP);
    setNextStateVariables(LEITURA);
    return LEITURA;
  }

}

static void setNextStateVariables(uint8_t nextState) {
  if (nextState == LEITURA) {
    gb::direcao = 'i';
    gb::atual_pos = 0;
    gb::flagSinalizaComeco = 1;
    gb::firstTime = 1;
    clearAllLeds();
    initTouch();
  }
}

void setNextSubStateVariables(uint8_t nextSubState) {
  if (nextSubState == LOGICA) {
    OCR1B = 65535;
    gb::subState = LOGICA;
  }
  else if (nextSubState == TREINAR) {
    OCR1B = 65535;
    qtdInterrupts = 0;
    FLAG_ISR = 'N'; //redundante, porem necessario!!! (testes empiricos)
    gb::subState = TREINAR;
  }
  else if (nextSubState == PAUSE) {
    TIMER_COUNT = TCNT1; //variavel auxiliar recebe valor de contagem do timer, para saber onde parou!
    gb::goPause = 1; //garante que essa variavel eh um na primeira vez q entra no estado PAUSE, para garantir a recursao nele
    gb::subState = PAUSE;
  }
  else if (nextSubState == INTERVALO) {
    qtdInterrupts = 0;
    gb::startNow = 0; //garante q essa variavel nao esta alterada
    logicaIntervalo();

    //necessario fazer esse teste pois pode existir tempo de descanso = 0!!!
    if (OCR1B <= TCNT1) { //Nao precisa disso na parte do treino pois provavelmente nao tera um valor no comparador tao pequeno! VER DPS CASO DE PROBLEMA
      FLAG_ISR = 'B'; //nao tem que esperar interrupcao para terminar o estado INTERVALO. //caso OCR1B == 0 (sem descanso):
    }
    else
      FLAG_ISR = 'N'; //redundante, porem necessario!!! (testes empiricos)

    //Serial.println(FLAG_ISR);
    gb::subState = INTERVALO;
  }
  else if (nextSubState == SINALIZA_COMECO) {

    //variavel auxiliar!!!! para nao ter q criar outra variavel, usa-se offset para controlar o startSinalizaComeco!!!
    if (gb::serieAtualCont == 0)
      offset = ledsSinalizaComeco - 1;
    else if (gb::serieAtual.intervalo == 0)
      offset = 1; //se offset for negativo vai acessar posicao inexistente do array de leds, por isso atribui offset = 1
    else
      offset = gb::serieAtual.intervalo >= ledsSinalizaComeco ? ledsSinalizaComeco - 1 : gb::serieAtual.intervalo - 1;

    OCR1B = 15625; //1s
    TIMSK1 = ( 1 << OCIE1B); //habilita comparacao apenas em B
    FLAG_ISR = 'N'; //redundante, porem necessario!!! (testes empiricos)
    gb::subState = SINALIZA_COMECO;
  }
  else { //STOP
    gb::flagSinalizaComeco = 1;
    gb::direcao = 'i';
    gb::atual_pos = 0; //tres variaveis devem ser zeradas ("reset" do arduino)
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
  gb::startNow = 0;
  gb::goPause = 0;

  final_pos = 0; //vai ser alterada antes de comecar o treino
  qtdIdas = 0;
  qtdMaxInterrupts = 0;
  qtdInterrupts = 0;
  FLAG_ISR = 'N';

  clearAllLeds();
  gb::fita.begin();
  gb::fita.show(); // Initialize all pixels to 'off'
  delay(100);
  gb::fita.setBrightness(10);

  initTimer(); //essa funcao ja inicializa OCR1B = MAX
  //setNextSubStateVariables(LOGICA); //nao chamar essa funcao pois a primeira vez que entra na logica, checkContTrechos nao deve ser chamada
  gb::subState = LOGICA; //mudar apenas o subState
  return TREINO;
}


uint8_t pause() {

  //Serial.println(F("Pause!"));

  if (gb::goPause == 0) { //so entra quando for despausado
    //Serial.println(F("Entrei no pause!"));

    //se entrou aqui é pq houve pause durante a execucao de alguma parte do algoritmo. A parte superior indica apenas o pause esperando o play do usuario na primeira serie!

    TCNT1 = TIMER_COUNT; //contador recebe valor que estava contando na hr do pause.
    if (offset == 23) {
      gb::subState = INTERVALO; //volta de onde tava no estado INTERVALO, ou seja, nao deve-se chamar setNextSubStateVariables()
      TIMSK1 = ( 1 << OCIE1B); //habilita comparador B de novo!
      FLAG_ISR = 'N';
      //Serial.println(FLAG_ISR);
    }
    else if (offset == 1 || offset == -1) {
      gb::subState = TREINAR; //se offset for igual a 1 ou -1, voltar para o estado TREINAR. volta de onde tava, ou seja, nao deve-se chamar setNextSubStateVariables()
      //Serial.println(F("Treinar"));
      //Serial.println(FLAG_ISR);
      //algum comparador deve ser habilitado!
      if (FLAG_ISR == 'A') {
        TIMSK1 = ( 1 << OCIE1A);
      }
      else TIMSK1 = ( 1 << OCIE1B);
      FLAG_ISR = 'N'; //zera variavel que foi usada para auxiliar essa logica!
      ////Serial.println(FLAG_ISR);
    }
    else { //caso em que houve despause durante sinalizaComeco!!! OFFSET NAO PODE SER NENHUM DOS VALORES ACIMA! Valores que ele assume durante sinComeco sao 19,14,9,4
      gb::subState = SINALIZA_COMECO;
      TIMSK1 = ( 1 << OCIE1B); //habilita comparador B de novo!
      FLAG_ISR = 'N';
      //Serial.println(FLAG_ISR);
    }
  }
  return TREINO;
}


uint8_t logica() { //inicializa a logica do timer. so deve ser chamada quando percorrer ate o ultimo pixel do trecho

  //Serial.print(F("LOGICA!  "));
  //Serial.println(atual_pos);

  int16_t pixelsTrecho = 0;

  //dividir logica entre idas e voltas
  if (gb::direcao == 'i') { //ida

    offset = 1; //incremento positivo do led
    pixelsTrecho = MAX_LEDS;
    setFinalPos(pixelsTrecho); //calcula-se posicao final do trecho

    float auxTime = (gb::serieAtual.TimeIda) / (pixelsTrecho - 1); //menos 1 pois sao numPixels - 1 intervalos de tempo
    //conversao do tamanho da piscina para pixels (multiplicar por 10). tempo em segs;

    if (auxTime * 15625 > 65535) { //15625 hz para prescale de 1024
      //se na menor escala a contagem nao caber, usar dois valores diferentes nos comparadores.
      OCR1A = (uint16_t)(round(auxTime * 15625) % 65535); //qtd de clocks da ultima interrupcao q der (apos n contagens maximas)
      qtdMaxInterrupts = (auxTime * 15625) / 65535;
    }
    else { //contagem cabe dentro de 15625hz
      OCR1A = (uint16_t)(round(auxTime * 15625)); //qtd de clocks
      qtdMaxInterrupts = 0;
    }
    gb::fita.setPixelColor(gb::atual_pos, gb::fita.Color(255, 0, 0)); //red
    gb::fita.show();

  }
  else { //volta
    offset = -1; //incremento negativo do led
    pixelsTrecho = MAX_LEDS;
    setFinalPos(-pixelsTrecho); //calcula-se posicao final do trecho. manda-se range de pixels negativo

    float auxTime = (gb::serieAtual.TimeVolta) / (pixelsTrecho - 1); //menos 1 pois sao numPixels - 1 intervalos de tempo
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

    gb::fita.setPixelColor(gb::atual_pos, gb::fita.Color(255, 0, 0)); //red
    gb::fita.show();

  }

  //Serial.println(OCR1A);
  //Serial.println(OCR1B);
  //Serial.println(qtdMaxInterrupts);

  if (qtdIdas == 0) { //se for a primeira vez que entrou no estado de logica ou o inicio de uma serie, inicializa sinalizaComeco
    if (gb::flagSinalizaComeco == 1 ) {
      setSinalizaComeco();
      setNextSubStateVariables(SINALIZA_COMECO);
    }
    return TREINO;
  }
  else { //inicio de uma repeticao que nao seja a primeira! necessario apenas habilitar os comparadores.
    setComp();
    setNextSubStateVariables(TREINAR);
    return TREINO;
  }
}

void logicaIntervalo() { //nao eh um sub estado mas esta muito atrelado ao estado INTERVALO

  //Serial.println(F("LOGICA INTERVALO"));



  //delay final de serie. para esse delay, faz sentido o usuario definir delay = 0, ja que ele pode querer fazer um treino progressivo (cada serie que ele manda o tempo muda)
  float auxTime = (gb::serieAtual.intervalo - ledsSinalizaComeco); //garantir no app que esse tempo seja zero ou maior igual a 5 se for delay entre series!!!!
  //alem disso, mais importante ainda, DEFINIR NO APP QUE O DELAY ENTRE TREINOS DEVE SER MAIOR QUE 5, senao a primeira serie do treino nao vai ter sinalizaComeco!!!  mudar isso dps caso precise

  if (auxTime <= 0) { //caso em que tempo de espera eh menor que tempo inicializaComeco
    OCR1B = 0;
    qtdMaxInterrupts = 1;

    /**************************

       MUDAR AQUI DPS ( FLAGSINALIZACOMECO )

     *************************/
    //gb::flagSinalizaComeco = 0; //a primeira repeticao da proxima serie que o arduino receber estara indicada para nao chamar sinalizaComeco!!!
  }

  //  else if (auxTime * 15625 > 65535) {
  //    OCR1A = (uint16_t)(round(auxTime * 15625) % 65535); //COMPA esta armazenando temporariamente esse valor. esse comparador nao eh usado nesse sub estado
  //    qtdMaxInterrupts = (auxTime * 15625 / 65535) + 1;//soma-se um pois deve entrar mais uma vez na funcao de interrupcao, na comp referente ao valor armazenado em A
  //  }
  //  else {
  //    OCR1B = (uint16_t)(round(auxTime * 15625));
  //    qtdMaxInterrupts = 1;
  //  }


  else {
    OCR1B = 15625; //1s
    qtdMaxInterrupts = (uint16_t)auxTime; //quantidade de segundos
    initDescanso();
    updateDescanso(gb::serieAtual.intervalo);
  }



  //Serial.println(FLAG_ISR);
  TCNT1 = 0;
  TIMSK1 = ( 1 << OCIE1B); //habilita comparacao apenas em B
  //Serial.println(FLAG_ISR);

}

uint8_t intervalo() {

  if (FLAG_ISR == 'B') {
    //Serial.println(FLAG_ISR);
    FLAG_ISR = 'N';
    qtdInterrupts++;

    if (qtdInterrupts == qtdMaxInterrupts || gb::startNow == 1) { //acabou o intervalo.
      /*Se startNow for atribuido pelo usuario, cancela-se o tempo de espera. Essa funcao ta habilitada para cancelar intervalos tanto de repeticoes quanto de series,
        entao eh importante que no app essa funcao so seja habilitada entre series!!!!!*/
      gb::startNow = 0;
      qtdInterrupts = qtdMaxInterrupts; //caso entre na funcao pelo startNow e um pause tenha ocorrido durante a execucao dessa condicao

      TIMSK1 &= ~(1 << OCIE1B); //desabilita ISR

      //OCR1B voltar a ser maximo em qualquer estado seguinte que for determinado

      //retornar para logica
      //Serial.println(F("Fim intervalo!!!"));
      setNextSubStateVariables(LOGICA);

    }
    else {
      updateDescanso(gb::serieAtual.intervalo - qtdInterrupts); //nao deve atualizar o tempo quando terminar o intervalo (inicializaComeco que irá mostrar esse numero)
    }
  }

  //IMPORTANTE: else if igual ao sub estado TREINAR!!!
  else if (gb::goPause == 1) { //testa se entrou no estado pause
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
    if (gb::atual_pos + offset <= MAX_LEDS - 1 && gb::atual_pos + offset >= 0)   gb::fita.setPixelColor(gb::atual_pos + offset, gb::fita.Color(255, 0, 0)); //red

    if (gb::atual_pos - offset <= MAX_LEDS - 1 && gb::atual_pos - offset >= 0)   gb::fita.setPixelColor(gb::atual_pos - offset, gb::fita.Color(0, 0, 0)); //apaga pixel anterior
    gb::atual_pos += offset;

    if (gb::atual_pos == final_pos) {
      TIMSK1 &= ~(1 << OCIE1A) & ~(1 << OCIE1B); //desabilita interrupcao

      qtdIdas++;
      if (gb::direcao == 'i') gb::direcao = 'v';
      else {
        //Serial.println(qtdIdas);
        gb::direcao = 'i';
        if (qtdIdas != 2 * gb::serieAtual.repeticoes) updateRepeticao(((qtdIdas) / 2) + 1); //quando a ultima ida for completada, nao deve-se chamar updateRepeticao!
      }

      //Serial.println(millis() - tempo);

      if (gb::atual_pos - offset <= MAX_LEDS - 1 && gb::atual_pos - offset >= 0)   gb::fita.setPixelColor(gb::atual_pos - offset, gb::fita.Color(0, 0, 0)); //apaga pixel anterior.
      //esta aqui apenas para detalhe de visualizacao

      if (qtdIdas == 2 * gb::serieAtual.repeticoes) { //verifica se ja terminou uma serie

        qtdIdas = 0;
        gb::serieAtualCont++;

        if (gb::serieAtualCont == gb::serieAtual.qtdSeries) { //se acabou a serie, retornar para leitura

          gb::serieAtualCont = 0;

          //Serial.println(F("Fim treino!!!"));
          setNextStateVariables(LEITURA);
          return LEITURA; //se a serie tiver sido executada

        }
        else setNextSubStateVariables(INTERVALO);
      }
      else { //senao, eh pq terminou apenas uma ida
        setNextSubStateVariables(LOGICA);
      }
    }
    else if (qtdMaxInterrupts > 0) { //caso em que o comparador B é usado. Deve-se desabilitar COMP em A!!!
      TIMSK1 = ( 1 << OCIE1B); //habilita comp apenas em B.
    }

    gb::fita.show();
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
  else if (gb::goPause == 1) { //testa se entrou no estado pause

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
    //Serial.println(F("SinalizaComecoooo"));
    FLAG_ISR = 'N';

    //acabou sinalizaComeco!
    if (offset == -2) {
      TIMSK1 &= ~(1 << OCIE1B); //desabilita interrupcao
      setFirstLed();//inicializa o timer agr (comeco da repeticao)
      //volta valor do offset para o estado TREINAR
      if (gb::direcao == 'i') offset = 1;
      else offset = -1;
      initRepeticao();
      updateRepeticao(1);
      setNextSubStateVariables(TREINAR);
    }
    else {
      int8_t aux = offset - 1;
      updateDescanso(offset);
      if (gb::direcao == 'i') {
        //Serial.println(F("SinalizaIda"));
        for (; offset > aux; offset--) {
          gb::fita.setPixelColor(offset, gb::fita.Color(0, 0, 0));
        }
      }
      else {
        for (; offset > aux; offset--) {
          gb::fita.setPixelColor(MAX_LEDS - offset - 1, gb::fita.Color(0, 0, 0));
        }
      }
      gb::fita.show();

      //quer dizer que acabou sinalizaComeco, mas eh necessario esperar mais 1 segundo para comecar o treino!
      if (offset == -1) offset--; //para evitar que tenha um pause e o offset valha -1. Iria dar problema no estado pause, na hr de decidir para qual estado retornar!!!
    }
  }
  else if (gb::goPause == 1) { //testa se entrou no estado pause
    TIMSK1 &= ~(1 << OCIE1B); //desabilita interrupcao
    FLAG_ISR = 'N';
    setNextSubStateVariables(PAUSE);
  }
  return TREINO;
}


/*********************
   FUNCOES AUXILIARES
 *********************/

void setFirstLed() { //na primeira serie e apos os delays, eh necessario chamar essa funcao, pois o primeiro led nao precisa esperar para ser aceso.
  //chamado toda vez que inicia uma repeticao tbm

  //Serial.println(F("setFirst"));

  gb::fita.setPixelColor(gb::atual_pos, gb::fita.Color(255, 0, 0));
  gb::fita.show();
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
  final_pos = gb::atual_pos + pixelsRange - offset; //offset eh positivo na ida e negativo na volta
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

  int ledsAux;
  if (gb::serieAtualCont == 0)
    ledsAux = ledsSinalizaComeco;
  else
    ledsAux = gb::serieAtual.intervalo >= ledsSinalizaComeco ? ledsSinalizaComeco : gb::serieAtual.intervalo;

  if (gb::direcao == 'i') {
    //Serial.println(F("setSinalizaIda"));
    for (int  i = 0; i < ledsAux; i++) {
      gb::fita.setPixelColor(i, gb::fita.Color(255, 0, 0));
    }
  }
  else {
    for (int  i = MAX_LEDS - ledsAux; i < MAX_LEDS ; i++) {
      gb::fita.setPixelColor(i, gb::fita.Color(255, 0, 0));
    }
  }
  gb::fita.show();
  TCNT1 = 0;
  initInicializaComeco();
  updateDescanso(ledsAux);

}

void clearAllLeds() {
  gb::fita.begin();
  gb::fita.show(); // Initialize all pixels to 'off'
  delay(100);
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


