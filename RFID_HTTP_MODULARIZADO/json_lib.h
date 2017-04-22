#ifndef JSON_LIB
#define JSON_LIB

#include "config.h"

//*****************************************************************************************************
//Prototipos

String check_json(String aux);
String parseJson(String parametro, char tipo, String jsonstring);
String update_parsing(String jsonstring, int flag);
String service_parsing(String jsonstring);
String authentication_parsing(String jsonstring);

#endif
