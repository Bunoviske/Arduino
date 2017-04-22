#include "AT.h"

void initialize_serial()
{
  static SoftwareSerial * esp8266 = new SoftwareSerial(2,3);

  esp8266->begin(115200);
}

String sendCommand(String command, const int timeout)
{
  
   String response = "";
   esp8266->print(command);
   esp8266->setTimeout(timeout);
   response = esp8266->readString();

   Serial.print(response);
   return response;
   
}




