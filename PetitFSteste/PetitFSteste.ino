// Petit FS test.
// For minimum flash use edit pffconfig.h and only enable
// _USE_READ and either _FS_FAT16 or _FS_FAT32

#include "PetitFS.h"
#include "PetitSerial.h"

PetitSerial PS;
// Use PetitSerial instead of Serial.
//#define Serial PS

// The SD chip select pin is currently defined as 10
// in pffArduino.h.  Edit pffArduino.h to change the CS pin.

FATFS fs;     /* File system object */
UINT nr;
char fileName[7];
char txt[] = ".TXT";
char escreve1[] = "1";


//------------------------------------------------------------------------------
void errorHalt(char* msg) {
  Serial.print("Error: ");
  Serial.println(msg);
  while (1);
}
//------------------------------------------------------------------------------
void test() {
  char buf[32];
  char escreve2[] = "ola";

  //Serial.println(strcat(nome, txt));


  // Initialize SD and file system.
  if (pf_mount(&fs)) errorHalt("pf_mount");

  // Open test file.
  if (pf_open("1.TXT") )errorHalt("pf_open");

  // Read file
  nr = 1;
  int cont = 0;
  while (nr != 0) {
    if (pf_read(buf, sizeof(buf), &nr)) errorHalt("pf_read");
    for (UINT i = 0; i < nr; i++) {
      if (buf[i] >= 48) { //somente numero
        cont++;
      }
      else {
        buf[i] = '\0';
        break;
      }
    }
  }
  Serial.println(buf);
  Serial.print("Caracteres: ");
  Serial.println(cont);


  //  if (pf_write(buf, (UINT)strlen(buf), &nr)) errorHalt("pf_write");
  //  if (pf_write(escreve1, (UINT)strlen(escreve1), &nr)) errorHalt("pf_write");
  if (pf_write(escreve1, sizeof(escreve1), &nr)) errorHalt("pf_write");
  if (pf_write(0, 0, &nr)) errorHalt("pf_write2");
  //
  //  delay(500);
  //  Serial.println("Segundo arquivo");
  //
  //  itoa(2, nome, 10);
  //  if (pf_open(strcat(nome, txt))) errorHalt("pf_open");
  //
  //  // Read file
  //  nr = 1;
  //  cont = 0;
  //  while (nr != 0) {
  //    if (pf_read(buf, sizeof(buf), &nr)) errorHalt("pf_read");
  //    for (UINT i = 0; i < nr; i++) {
  //      if (buf[i] != 0) {
  //        cont++;
  //        Serial.print(buf[i]);
  //      }
  //    }
  //  }
  //  Serial.print("\nCaracteres: ");
  //  Serial.println(cont);
  //
  //  if (pf_write(buf, (UINT)strlen(buf), &nr)) errorHalt("pf_write");
  //  if (pf_write(escreve2, (UINT)strlen(escreve2), &nr)) errorHalt("pf_write");
  //  if (pf_write(escreve2, (UINT)strlen(escreve2), &nr)) errorHalt("pf_write");
  //  if (pf_write(0, 0, &nr)) errorHalt("pf_write2");




}
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  test();
 // initSD();
 // readSD();
 // writeSD();
  //checkFileNumber();
  Serial.println("\nDone!");
}
void loop() {}

void readSD() {
  if (pf_open("1.TXT")) {
    //#ifdef DEBUG
    errorHalt("pf_open");
    //#endif
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
  Serial.print("txt 1: ");
  Serial.println(buf);
}

int checkFileNumber() {

  if (pf_open("0.TXT")) {
    //#ifdef DEBUG
    errorHalt("pf_open");
    //#endif
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
  Serial.print("Cont: ");
  Serial.println(buf);

  //incrementa contador que está separado em um arquivo!!!
  int cont = atoi(buf);
  char contNovo[6];
  itoa(cont + 1, contNovo, 10);
  //Serial.println(contNovo);
  //pf_write(contNovo, sizeof(contNovo), &nr) ;
  //pf_write(0, 0, &nr);
  return cont;
}

void writeSD() {

  // Open test file.
  itoa(checkFileNumber(), fileName, 10);
  //Serial.println(strcat(fileName, txt));
  if (pf_open(strcat(fileName, txt))) {
    //#ifdef DEBUG
    errorHalt("pf_open");
    //#endif
  }
  //Serial.println(fileName);

  int tempo = (millis() / 1000) / 60; //em minutos
  char buf[32];
  itoa(tempo, buf, 10);
  pf_write(escreve1, sizeof(escreve1), &nr);
  //pf_write('\n', 1, &nr);
  //pf_write(&gb::serieAtual, sizeof(gb::serieAtual), &nr);
  pf_write(0, 0, &nr);

}

void initSD() {


  // Initialize SD and file system.
  if (pf_mount(&fs)) {
    //#ifdef DEBUG
    errorHalt("pf_mount");
    //#endif
  }

}
