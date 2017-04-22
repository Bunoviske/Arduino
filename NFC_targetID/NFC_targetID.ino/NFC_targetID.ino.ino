#include <SoftwareSerial.h>
#include <PN532.h>
#include <SPI.h>

/*Chip select pin can be connected to D10 or D9 which is hareware optional*/
/*if you the version of NFC Shield from SeeedStudio is v2.0.*/
#define PN532_CS 10
PN532 nfc(PN532_CS);

SoftwareSerial myserial(2, 3);

uint32_t id_antes = 0;
unsigned long tempo = 0;

void setup(void) {
  Serial.begin(9600);
  myserial.begin(9600);
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  // configure board to read RFID tags and cards
  nfc.SAMConfig();
}


void loop(void) {
  uint32_t id;
  // look for MiFare type cards

  id = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A);

  if (id != 0) {
    if (id != id_antes) {
      myserial.print(id);
      delay(3000);
      Serial.print("Read card #");
      Serial.println(id);
      id_antes = id;
      tempo = millis();
    }
    else if ((millis() - tempo) >= 10000) {
      myserial.print(id);
      delay(3000);
      Serial.print("Read card #");
      Serial.println(id);
      tempo = millis();
    }
  }
}
