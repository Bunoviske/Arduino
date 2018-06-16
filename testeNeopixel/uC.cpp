#include "uC.h"

static File myFile;

void writeSD() {

  myFile = SD.open("dados.txt", FILE_WRITE);
  if (myFile) {
    myFile.println("testing 1, 2, 3.");
    char c = myFile.read();
    myFile.available();
  }
  myFile.close();

}

void initSD() {

  //#define SD_CS   10 //pino de selecao do cartao SD

  if (!SD.begin(10)) {
    //Serial.println("initialization failed!");
    return;
  }
}

void uC_config() {

#ifdef DEBUG
  Serial.begin(115200);
#endif

  configTouch();
  initSD();
  writeSD();
  setNextStateVariables(); //proximo estado: leitura

}

static void setNextStateVariables() {

}
