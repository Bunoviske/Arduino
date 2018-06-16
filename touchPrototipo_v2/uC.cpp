#include "uC.h"

static FATFS fs;     /* File system object */
static char txt[] = ".TXT";
static char fileName[10];
static UINT nr;

#ifdef DEBUG
void errorHalt(char* msg) {
  Serial.print("Error: ");
  Serial.println(msg);
}
#endif


int checkFileNumber() {

  if (pf_open("0.TXT")) {
#ifdef DEBUG
    errorHalt("pf_open");
#endif
  }

  char buf[32];
  nr = 1;
  while (nr != 0) {
    if (pf_read(buf, sizeof(buf), &nr)) errorHalt("pf_read");
    for (UINT i = 0; i < nr; i++) {
      if (buf[i] < 48) { //somente numero
        buf[i] = '\0';
        i = nr; //break
      }
    }
  }
#ifdef DEBUG
  Serial.print("Cont: ");
  Serial.println(buf);
#endif

  //incrementa contador que está separado em um arquivo: 0.TXT !!!
  int cont = atoi(buf);
  char contNovo[6];
  itoa(cont + 1, contNovo, 10);
  pf_write(contNovo, sizeof(contNovo), &nr) ;
  pf_write(0, 0, &nr);
  return cont;
}

void writeSD() {

  // Open test file.
  itoa(checkFileNumber(), fileName, 10);
  if (pf_open(strcat(fileName, txt))) {
#ifdef DEBUG
    errorHalt("pf_open");
#endif
  }

  char buf[6];
  unsigned long tempo = millis();
  pf_write(tempo, sizeof(tempo), &nr);
  //pf_write('\n', 1, &nr);
  //pf_write(&gb::serieAtual, sizeof(gb::serieAtual), &nr) ;
  pf_write(0, 0, &nr);

}

void initSD() {


  // Initialize SD and file system.
  if (pf_mount(&fs)) {
#ifdef DEBUG
    errorHalt("pf_mount");
#endif
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
  gb::subState = GET_USER_INFO_MAIN;
}
