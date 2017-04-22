#include "data.h"

void data_warning(String data_value, const int timeout, int connectionId)
{
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += data_value.length();
  cipSend += "\r\n";
  sendCommand(cipSend, timeout);
}

String send_data(String data_value, const int timeout, int connectionId)
{
  String resposta;
  
  data_warning(data_value, timeout, connectionId);
  
  resposta = sendCommand(data_value, timeout);
  
  return resposta;
}




