#include <Adafruit_NeoPixel.h>

#include <avr/power.h>
#include <avr/interrupt.h>

#define PIN            6
#define NUMPIXELS      50

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);

volatile int k = 0;

void setup() {
  Serial.begin(115200);
  pixels.begin(); // This initializes the NeoPixel library.
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  pixels.setBrightness(10);

  sinaliza_comeco();
  timerSetup();
}

volatile uint16_t hello = 0;
volatile uint16_t contInterrupt = 0;

void loop() {

  if (k == NUMPIXELS) {
    TIMSK1 = 0;
    sinaliza_comeco();
    TCNT1  = 0;
    k = 0;
    TIMSK1 |= (1 << OCIE1B);
  }
  //  sinaliza_comeco();
  //  for (int i = 0; i < NUMPIXELS; i++) {
  //
  //    pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Moderately bright green color.
  //
  //    if (i - 1 >= 0) {
  //      pixels.setPixelColor(i - 1, pixels.Color(0, 0, 0));
  //    }
  //
  //    pixels.show(); // This sends the updated pixel color to the hardware.
  //
  //    delay(100); // Delay for a period of time (in milliseconds).
  //
  //  }
}

void timerSetup() {
  cli();//stop interrupts

  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0

  // turn on CTC mode
  //TCCR1B |= (1 << WGM12); //zera o contador quando a comparacao é verdadeira

  //1024 prescaler
  //TCCR1B |= (1 << CS12) | (1 << CS10);

  //256 prescaler
  TCCR1B |= (1 << CS12); //62.5Khz

  // set compare match register for 1hz increments
  OCR1A = 58900;// = (16*10^6) / (1*1024) - 1 (must be <65535 == 2^16)
  OCR1B = 65535; //max possivel

  // enable timer compare interrupt
  TIMSK1 |= ( 1 << OCIE1A);

  sei();//allow interrupts
}

void sinaliza_comeco() {

  for (int i = 19; i >= 0; i--) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));
  }
  pixels.show();
  delay(1000);
  int j = 19;
  int aux;
  for (int i = 0; i < 4; i++) {
    aux = j - 5;
    for (; j > aux; j--) {
      pixels.setPixelColor(j, pixels.Color(0, 0, 0));
      pixels.show();
    }
    delay(1000);
  }
}



ISR(TIMER1_COMPA_vect) {
  TCNT1  = 0;

  if (k < NUMPIXELS) { // if redundante
    pixels.setPixelColor(k, pixels.Color(255, 0, 0)); // Moderately bright green color.
    if (k - 1 >= 0) {
      pixels.setPixelColor(k - 1, pixels.Color(0, 0, 0));
    }
    pixels.show(); // This sends the updated pixel color to the hardware
    k++;

    TIMSK1 &= ~(1 << OCIE1A);
    TIMSK1 |= (1 << OCIE1B);
  }
}


ISR(TIMER1_COMPB_vect) {
  TCNT1  = 0;

  if (contInterrupt == 1) {
    TIMSK1 &= ~(1 << OCIE1B);
    TIMSK1 |= (1 << OCIE1A);
    contInterrupt = 0;
  }
  else {
    contInterrupt++;
  }

  //  if (k < NUMPIXELS) { // if redundante
  //    pixels.setPixelColor(k, pixels.Color(0, 255, 0)); // Moderately bright green color.
  //    if (k - 1 >= 0) {
  //      pixels.setPixelColor(k - 1, pixels.Color(0, 0, 0));
  //    }
  //    pixels.show(); // This sends the updated pixel color to the hardware
  //    k++;
  //  }
}



