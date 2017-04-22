#include "sensor.h"

unsigned int readSensor()
{
  String receivedTagCode;
  unsigned int tmp;

  if (Serial1.available()) {
      receivedTagCode = Serial1.readString(); //Le String do cartao
      String str = receivedTagCode.substring(5,11); //Quebea String, pega apenas dados do carto ignorando CheckSum
      tmp = hexToDec(str); //Converte dados
      Serial.println(tmp);
      return tmp;
  }
  return 0;
}

unsigned int hexToDec(String hexString) {

  unsigned int decValue = 0;
  int nextInt;

  for (int i = 0; i < hexString.length(); i++) {

    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);

    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}
