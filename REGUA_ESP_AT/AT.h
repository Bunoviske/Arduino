#ifndef AT_H
#define AT_H  

#include <SoftwareSerial.h>
#include <Arduino.h>

    static SoftwareSerial * esp8266 = new SoftwareSerial(2,3);

    //deve usar static para ser declarado apenas uma vez e nao perder suas propriedades, como o esp8266->begin().
    void initialize_serial();
    String sendCommand(String command, const int timeout);

#endif
