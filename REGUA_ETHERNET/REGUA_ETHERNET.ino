
//*****************************************************************************************************
//HEADER's
#include <SPI.h>
#include <Ethernet.h>


//*****************************************************************************************************
//DEFINE's


//autenticacao
#define host_name               "Arduino_Monitoramento_Energia"

#define refresh_rate            10000

#define NUM_SERVICOS            1 //num servicos define tanto o num de action id quanto num de item id
#define TAM_TOKEN_MAX           15
#define TAM_ACTION_ID_MAX       15 //se for um dispositivo que tem mais de um action id, dobrar esse valor
#define TAM_ITEM_ID_MAX         15 //mesma coisa com o item id

#define pinoSensor              A0






//*****************************************************************************************************
//GLOBAIS

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
String mac_aux = "DE:AD:BE:EF:FE:ED";

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
float readSensor();

//*****************************************************************************************************
//Setup
void setup () {
  initSerial(9600);
  //initGPIO();

  nome_servico[0] = "potencia";


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
    value[data] = readSensor();
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

float readSensor()
{
  int sensorValue_aux = 0;
  float valorSensor = 0;
  float valorCorrente = 0;
  float voltsporUnidade = 0.004887586;// (Maximo valor de tensao do arduino / resolucao de bits do conversor AD) 5%1023
  // Para ACS712 de  5 Amperes usar 0.185
  // Para ACS712 de 10 Amperes usar 0.100
  //  Para ACS712 de 30 Amperes usar 0.066
  float sensibilidade = 0.066;

  //Tensao da rede AC 110 Volts e na verdade (127 volts)
  int tensao = 220;
  String receivedTagCode;
  unsigned int corrente;

  for (int i = 10000; i > 0; i--) {
    // le o sensor no pino analogico A0 e ajusta o valor lido, ja que a saída do sensor é (1023)vcc/2 para corrente = 0
    sensorValue_aux = (analogRead(pinoSensor) - 510);
    // somam os quadrados das leituras.
    valorSensor += pow(sensorValue_aux, 2);
    delay(1);
  }

  // finaliza o calculo da média quadratica e ajusta o valor lido para volts
  valorSensor = (sqrt(valorSensor / 10000)) * voltsporUnidade;
  // calcula a corrente considerando a sensibilidade do sernsor (66 mV por amper)
  valorCorrente = (valorSensor / sensibilidade);

  //tratamento para possivel ruido
  //O ACS712 para 30 Amperes é projetado para fazer leitura
  // de valores alto acima de 0.25 Amperes até 30.
  // por isso é normal ocorrer ruidos de até 0.20A
  //por isso deve ser tratado



  valorSensor = 0;

  if (valorCorrente <= 0.10) {
    Serial.print("Valor corrente média: 0.00");
    Serial.println("       Pontecia consumuida: 0.00");
  }


  else {
    Serial.print("Valor corrente média: ");
    Serial.print(valorCorrente);
    Serial.print("       Pontecia consumuida:");
    Serial.println(valorCorrente * tensao);
  }
  return valorCorrente * tensao;

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
}

