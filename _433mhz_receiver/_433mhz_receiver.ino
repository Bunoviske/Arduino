
#include <VirtualWire.h>

#define receive_pin 3

void setup()
{
    Serial.begin(9600);	// Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_rx_pin(receive_pin);
    vw_setup(2000);	 // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
    pinMode(9, OUTPUT);
}

void loop()
{
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    int i;
    static int valor;

    if (vw_have_message()){
      delay(100);    
      if (vw_get_message(buf, &buflen))
      {
      	Serial.print("Recebido: ");
      	char char_array[buflen];
      	for (i = 0; i < buflen; i++)
      	{
      	    Serial.println((char)buf[i]);
            char_array[i]  = (char)buf[i];
      	    
      	}
       char_array[i] = '\0';
       valor = atoi(char_array);
       Serial.println(valor);
      }
   }
   if (valor==1){
    digitalWrite(9, HIGH);
   }
   else{
    digitalWrite(9, LOW);
   }
}
