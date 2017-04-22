
//*****************************************************************************************************
//HEADER's
#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>

//*****************************************************************************************************
//DEFINE's


//autenticacao
#define host_name               "Arduino_Monitoramento_Clima"

#define refresh_rate            10000

#define NUM_SERVICOS            3 //num servicos define tanto o num de action id quanto num de item id
#define TAM_TOKEN_MAX           15
#define TAM_ACTION_ID_MAX       15 //se for um dispositivo que tem mais de um action id, dobrar esse valor
#define TAM_ITEM_ID_MAX         15 //mesma coisa com o item id

// DHT
#define DHTPIN                  7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);





//*****************************************************************************************************
//GLOBAIS

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
String mac_aux = "DE:AD:BE:EF:FE:DE";

char *server = "172.16.9.61";
int porta = 5510;


EthernetClient update; //client responsavel pela atualizacao em arguments

String id_token = "";
//String action_id = ""; // para mais de uma servico String action_id[n], com n == numero de servicos
//String item_id = ""; //   mesma coisa para o item_id
String atualiza = "";
String autent = "";
String servicos = "";

String action_id[NUM_SERVICOS]; // para mais de uma servico String action_id[n], com n == numero de servicos
String item_id[NUM_SERVICOS]; //   mesma coisa para o item_id
String nome_servico[NUM_SERVICOS];

int pos_final_token = 0;

int flag_global = 0;
String metodo = "POST ";

//ponteiro de função da máquina de estados. Ele aponta sempre para a função da máquina de estados que deve ser executada
void (*FSM_SBRT)();
void (*FSM_SBRT_anterior)();

//*****************************************************************************************************
//Prototipos

//FSM
void StateInit(void);       //função que representa o estado inicial da máquina de estados
void StateUpdate(void);


//Metodos privados

void initSerial(unsigned int baudrate);
void initGPIO();
unsigned int readSensor();

//*****************************************************************************************************
//Setup
void setup () {
  initSerial(9600);
  //initGPIO();

  nome_servico[0] = "temperatura";
  nome_servico[1] = "humidade";
  nome_servico[2] = "luminosidade";

  //INICIALIZA FSM
  FSM_SBRT = StateInit;

}


//*****************************************************************************************************
//LOOP
void loop() {
  (*FSM_SBRT)();
}


//*****************************************************************************************************
//Implementação FSM
void StateInit()
{
  do {
    if (Ethernet.begin(mac) != 0) {
      Serial.println("Ethernet conectado");
      delay(1000);
    }
    else {
      Serial.println("Failed to configure Ethernet using DHCP");
      // somente pra ip static Ethernet.begin(mac, ip);
    }
    delay(5000);
  } while (Ethernet.begin(mac) == 0);

  //TROCA DE ESTADO
  FSM_SBRT = StateUpdate;
}


void StateUpdate()
{
  int data = 0;
  int value[NUM_SERVICOS];
  for (data = 0; data < NUM_SERVICOS; data++) {
    value[data] = readSensor(data);
    while (update.connect(server, porta) != 1) {
      Serial.println("Failed to connect update, trying again in 5 seconds...");
      delay(5000);
    }
    Serial.println("Conectado no servidor");
    String atualizacao = mac_aux;
    atualizacao += ';';
    atualizacao += convert_localIP(Ethernet.localIP());
    atualizacao += ';';
    atualizacao += host_name;
    atualizacao += ';';
    atualizacao += nome_servico[data];
    atualizacao += ';';
    atualizacao += (String)value[data];
    atualizacao += ';';
    atualizacao += refresh_rate / 1000;
    atualizacao += ';';
    atualizacao += 555;
    Serial.println(atualizacao);
    update.print(atualizacao);
    delay(5000);

    while (update.available()) {
      String resposta = update.readString();
      Serial.println(resposta);
    }
    update.stop();
  }

  FSM_SBRT = StateUpdate;
  delay(refresh_rate);
}




//*****************************************************************************************************
//METODOS PRIVADOS

float readSensor(int data)
{
  float value;
  if (data == 0) {
    value = dht.readTemperature();
    Serial.print("Temperatura: ");
    Serial.println(value);
  }
  else if (data == 1) {
    value = dht.readHumidity();
    Serial.print("Humidade: ");
    Serial.println(value);

  }
  else {
    value = analogRead(A0);
    Serial.print("raw value: ");
    Serial.print(value);
    //transformacao para que 0 seja baixa luminosidade e 1024 seja alta luminosidade
    value = 1024 - value;
    //transformacao de porcentagem
    value = (100 * value) / 1024;
    Serial.print("Luminosidade: ");
    Serial.print(value);
    Serial.println("%");
  }

  return value;

}

String convert_localIP(IPAddress ip) {

  String localip = "";

  for (int i = 0; i < 4; i++) {
    localip += i ? "." + String(ip[i]) : String(ip[i]);
  }

  return localip;
}



//INICIALIZA A COMUNICAÇão SERIAL
void initSerial(unsigned int baudrate)
{
  Serial.begin(baudrate);
}

//INICIALIZA pinos GPIO
void initGPIO()
{
  dht.begin();
}

