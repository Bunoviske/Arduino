#include <Wire.h>
char buff[100];
int flag = 0;
int contGlobal = 0;

void setup() {

  Wire.begin(1);              
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);    // register event
  Serial.begin(115200);           // start serial for output


  Serial.println("Comecando o codigo");
}

void loop() {
//  flag = 0;
//  delay(20000);
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howmany) {
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    if ( c != '&') {
      buff[contGlobal] = c;
      contGlobal++;
    }
    else {
      buff[contGlobal] = '\0';
      Serial.println(buff);
      Serial.println(contGlobal);
      contGlobal = 0;

    }
  }

}

void requestEvent() {
  if (flag == 0) {
    Wire.write("555"); // respond with message of 6 bytes
    Serial.println("Callback request");
    //flag = 1;
  }
  // as expected by master
}
