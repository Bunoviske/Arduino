#ifndef DATA_H
#define DATA_H

#include "AT.h" 

void data_warning(String,const int timeout, int connectionId);
String send_data(String, const int timeout, int connectionId);

#endif
