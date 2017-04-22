//*****************************************************************************************************
//HEADER's
#include "state_machine.h"




//Prototipos
void initSerial(unsigned int baudrate);
void initGPIO();

//*****************************************************************************************************
//Setup
void setup () {
  initSerial(9600);
  //initGPIO();

  //INICIALIZA FSM
  FSM_SBRT = StateInit;
}


//*****************************************************************************************************
//LOOP
void loop(){
//  int estado = (*FSM_SBRT)();
//
//  switch (estado)
//  {
//    case READ_EPROM:
//      FSM_SBRT = StateReadEprom;
//      break;
//    case AUTENT:
//      FSM_SBRT = StateAuthentication;
//      break;
//    case SERVICE:
//      FSM_SBRT = StateService;
//      break;            
//    case UPDATE:
//      FSM_SBRT = StateUpdate;
//      break;      
//    
//  }
   (*FSM_SBRT)();
}


//INICIALIZA A COMUNICAÇão SERIAL 
void initSerial(unsigned int baudrate)
{
  Serial.begin(baudrate);
  Serial1.begin(baudrate);
}

//INICIALIZA pinos GPIO
void initGPIO()
{

}







