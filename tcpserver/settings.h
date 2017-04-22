#ifndef SETTINGS_H
#define SETTINGS_H


#include "server_connection.h"
#include "data.h"

#define SSID "dlink-9FC8"
#define PSWD "xpqap49352"
#define porta 80
#define protocolo "TCP"

//declarar se for servidor
Server_Connection * esp_server = new Server_Connection(SSID, PSWD, 1, 1, protocolo, porta);

Data * dado = new Data();


#endif
