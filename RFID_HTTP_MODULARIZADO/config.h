#ifndef CONFIG
#define CONFIG

//*****************************************************************************************************
//HEADER's
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <Arduino.h>


//*****************************************************************************************************
//DEFINE's

#define PUT                     0
#define POST                    1
#define GET                     2

#define ACTION_ID_RFID          "196"

//autenticacao
#define host_name               "Arduino_RFID"
#define description_autent      "Arduino_Modulo_Rfid"
#define id_chipset              123
#define id_processor            123
#define hd_serial               123

//servico 1
#define nome_service_servico1   "rfid" //nome servico 1
#define inf_type                "string" //tipo variável
#define scpdurl                 "172,16,9,123"
#define description_service     "Autenticacao" //descricao
#define control_url             "172,16,9,123"
#define event_sud_url           "172,16,9,123"
#define refresh_rate            30000

//update servico
#define nome_update_servico1    "rfid"


#define TAM_TOKEN               42
#define TAM_ACTION_ID_MAX       5 //se fo um dispositivo que tem mais de um action id, dobrar esse valor
#define TAM_ITEM_ID_MAX         5 //mesma coisa com o item id

/*flags para mais de 1 servico
#define flag_servico1           0
#define flag_servico2           1
*/

//*****************************************************************************************************
//GLOBAIS

static  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static  String mac_aux = "0xDE,0xAD,0xBE,0xEF,0xFE,0xED";

static  char server[] = "raise.uiot.org";
static  IPAddress ip(172,16,9,123);

static  EthernetClient autenticacao; //client responsavel pela autenticacao
static  EthernetClient servico; //client responsavel pelo cadastro de servicos
static  EthernetClient update; //client responsavel pela atualizacao em arguments

extern String  jsonString;
static  String id_token = "";
static  String action_id = ""; // para mais de uma servico String action_id[n], com n == numero de servicos
static  String item_id = ""; //   mesma coisa para o item_id
static  String atualiza = "";
static  String autent = "";
static  String servicos = "";

static  int flag_global = 0;
static  String metodo = "POST ";

static  unsigned long previous_time = 0;
static  unsigned long current_time = 0;

//*****************************************************************************************************
//Prototipos

String autentURL();
String expiredTokenURL();
String serviceURL();
String updateURL(unsigned int return_value, int i);



#endif
