#include <avr/interrupt.h>

void setup() {

  cli();//stop interrupts

  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // turn on CTC mode
  TCCR1B |= (1 << WGM12); //zera o contador quando a comparacao é verdadeira
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);

  // set compare match register for 1hz increments
  OCR1A = 1562;// = (16*10^6) / (1*1024) - 1 (must be <65536 == 2^16)

  // enable timer compare interrupt
  TIMSK1 |= ( 1 << OCIE1A);

  pinMode(13, OUTPUT);
  Serial.begin(9600);


  sei();//allow interrupts

}//end setup

void loop() {
  Serial.println("Ola");
  delay(1000);
}

ISR(TIMER1_COMPA_vect) {
  //timer1 interrupt 1Hz toggles pin 13 (LED)
  //generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  digitalWrite(13, digitalRead(13) ^ 1 );

  }




