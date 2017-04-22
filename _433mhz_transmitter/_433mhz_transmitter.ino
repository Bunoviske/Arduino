#include <VirtualWire.h>

#define transmit_pin 3
char msg[1];
    

void setup()
{
    pinMode(13,OUTPUT);  
    Serial.begin(9600);	  // Debugging only
    Serial.println("setup");
    vw_set_tx_pin(transmit_pin); 
    vw_setup(2000);	 // Bits per sec
    msg[0] = '1';
}

void loop()
{

    

    digitalWrite(13, true); // Flash a light to show transmitting
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);
    delay(5000);
    if (msg[0] == '1'){
      msg[0] = '0';
    }
    else{
      msg[0] = '1';
    }
}
