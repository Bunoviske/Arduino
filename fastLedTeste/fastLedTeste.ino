//#define ALLOW_INTERRUPTS 0
#include "FastLED.h"

#define NUM_LEDS 50
#define DATA_PIN 0



CRGB fita[NUM_LEDS]; //modo normal ---->usar BGR
//CRGBArray<500> leds; //setMode  ---->usar GRB

int atual_pos = 0;
int8_t offset = 1;
int final_pos = 49;
char direcao = 'i';
unsigned long tempo = 0;
volatile char FLAG_ISR = 0;

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  //delay(2000);
  //Serial.begin(115200);
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(3,HIGH);
  digitalWrite(5, LOW); //garante que a raia ta desligada

  FastLED.addLeds<WS2811, DATA_PIN, BRG>(fita, NUM_LEDS); //800KHZ
  FastLED.setBrightness(255);

//  for (int i = 0; i < 4; i++){
//    fita[i] = CRGB::Blue;
//  }
//  FastLED.show();
//  delay(100000);
  digitalWrite(5, HIGH);  
  initTimer();
  OCR1A = 1594; //15s total
  setSinalizaComeco();
  startSinalizaComeco();

  setFirstLed();

  

}
void loop() {
  state();


  //  for (int whiteLed = 1; whiteLed < NUM_LEDS; whiteLed++) {
  //    // Turn our current led on to white, then show the leds
  //    leds[whiteLed] = CRGB::Blue;
  //
  //    // Show the leds (only one of which is set to white, from above)
  //    FastLED.show();
  //
  //    // Wait a little bit
  //    delay(100);
  //
  //    // Turn our current led back to black for the next loop around
  //    leds[whiteLed] = CRGB::Black;
  //  }

  //  int i;
  //  FastLED.clear();
  //  sinaliza_comeco();
  //  for (i = 0; i < NUM_LEDS - 1; i++) {
  //    leds(i, i + 1) = CRGB::Red;
  //    FastLED.delay(100);
  //
  //    leds(i, i + 1) = CRGB::Black;
  //  }
  //  for (; i > 0; i--) {
  //    leds(i, i - 1) = CRGB::Red;
  //    FastLED.delay(100);
  //    leds(i, i - 1) = CRGB::Black;
  //  }

}

void state() {
  substate();
}

void substate() {
  treinar();
}
void treinar() {
  if (FLAG_ISR == 'A') {
    FLAG_ISR = 0;

    if (atual_pos + offset <= NUM_LEDS - 1 && atual_pos + offset >= 0)   fita[atual_pos + offset] = fita[atual_pos]; //acende de dois em dois pixels
    if (atual_pos - offset <= NUM_LEDS - 1 && atual_pos - offset >= 0)   fita[atual_pos - offset] = CRGB::Black; //apaga pixel anterior
    atual_pos += offset;

    if (atual_pos == final_pos) { //FIM DO TRECHO
      if (atual_pos - offset <= NUM_LEDS - 1 && atual_pos - offset >= 0)   fita[atual_pos - offset] = CRGB::Black; //apaga pixel anterior
      tempo = millis() - tempo;
      TIMSK1 &= ~(1 << OCIE1A);
      //Serial.println(tempo);
      delay(1000);
      atual_pos = 0;
      setFirstLed();
      //      offset = -1;
      //      final_pos = 0;
      //      //setFirstLed();
      //      tempo = millis();
      //      TIMSK1 = ( 1 << OCIE1A);
    }
    FastLED.show();
  }
}

void sinaliza_comeco() {
  //  leds(1, 25) = CRGB::Red;
  //  //FastLED.delay(1000);
  //  for (int i = 25; i >= 1; i-=5) {
  //    leds(i,i-5) = CRGB::Black;
  //    FastLED.delay(1000);
  //  }
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

  TIMSK1 &= ~(1 << OCIE1A) & ~(1 << OCIE1B); //comparadores comecam desabilitados. Normalmente sao habilitados nos sub estados treinar/intervalo

  sei();//allow interrupts

}

void setFirstLed() {

  fita[atual_pos] = CRGB::Green; //o primeiro pixel (posicao 0 ou NUMPIXELS - 1) smp deve receber verde nessa funcao.
  FastLED.show();
  tempo = millis();
  TCNT1 = 0;
  TIMSK1 = ( 1 << OCIE1A);

}

void setSinalizaComeco() {

  if (direcao == 'i') {
    for (int i = 19; i >= 0; i--) {
      fita[i] = CRGB::Green;
    }
  }
  else {
    for (int  i = (NUM_LEDS - 20); i <= NUM_LEDS - 1; i++) {
      fita[i] = CRGB::Green;
    }
  }
  FastLED.show();

}

void startSinalizaComeco() { //tirar esse delay dps!!!
  if (direcao == 'i') {
    int j = 19;
    int aux;
    for (int i = 0; i < 4; i++) {

      aux = j - 5;
      for (; j > aux; j--) {
        fita[j] = CRGB::Black;
      }

      FastLED.show();
      delay(1000);


    }
  }
  else {
    int j = (NUM_LEDS - 20);
    int aux;
    for (int i = 0; i < 4; i++) {
      aux = j + 5;
      delay(1000);
      for (; j < aux; j++) {
        fita[j] = CRGB::Black;
      }
      FastLED.show();
    }
  }
}

void clearAllLeds() {
  digitalWrite(5, LOW);
  FastLED.addLeds<WS2811, DATA_PIN, BRG>(fita, 250); //todos os 250 leds (maximo possivel)
  FastLED.clear();
  FastLED.show();
  digitalWrite(5, HIGH);
}

ISR(TIMER1_COMPA_vect) {
  TCNT1  = 0;
  FLAG_ISR = 'A';
}

