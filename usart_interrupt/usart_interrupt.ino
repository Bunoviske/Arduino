#include <avr/interrupt.h>
#include <avr/io.h>

#define USART_BAUDRATE 9600



void setup()
{
  cli();//stop interrupts
  //Serial.begin(9600);
  pinMode(13, OUTPUT);

  UBRR0L = USART_BAUDRATE & 0xff;
  UBRR0H = USART_BAUDRATE >> 8;
  UCSR0C |= (1 << UCSZ00) | (1 << USBS0); // Use 8-bit character sizes
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);   // Turn on the transmission, reception, and Receive interrupt
  sei();
}

void loop()
{

}

ISR(USART_RX_vect)
{
  digitalWrite(13, digitalRead(13) ^ 1);   // set the LED on
}
