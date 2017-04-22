#ifndef STATE_MACHINE
#define STATE_MACHINE

#include "json_lib.h"
#include "sensor.h"

//ponteiro de função da máquina de estados. Ele aponta sempre para a função da máquina de estados que deve ser executada
static void (*FSM_SBRT)();
static void (*FSM_SBRT_anterior)();

String jsonString;

//*****************************************************************************************************
//Prototipos

//FSM
void StateInit(void);       
void StateReadEprom(void);
void StateAuthentication(void);
void StateService(void);
void StateUpdate(void);



#endif
