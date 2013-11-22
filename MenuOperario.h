#include "General.h"


#ifndef MENUOPERARIO_H
    #define MENUOPERARIO_H

 //////////////////////////////////////////////////////////////    
//INCLUDES
//////////////////////////////////////////////////////////////
	
	
//////////////////////////////////////////////////////////////    
//DEFINES
//////////////////////////////////////////////////////////////
#define DIR_CNT_DESPIERTES              0x0000
#define DIR_DESPIERTES_POR_MEDIDA       0x0002
#define DIR_CNT_MUESTRAS_TOMADAS        0x0004
#define DIR_MUESTRAS_POR_ENVIO_MODEM    0x0006
#define DIR_ESTADO_SISTEMA              0x0008
#define DIR_ALARMA_SMS                  0x0009
#define DIR_TLF_ALARMA                  0x000A
#define DIR_PIN_SIM_MODEM               0x0013

#define ENVIOS_SMS_ACTIVADO             0x01
#define ENVIOS_SMS_DESACTIVADO          0x00

#define SELECCIONANDO_CIFRA             'S'
#define MODIFICANDO_CIFRA               'M'

#define ID_PRIMERA_CIFRA                0x00
#define ID_ULTIMA_CIFRA                 0x08

#define ID_PRIMERA_CIFRA_PIN            0x00
#define ID_ULTIMA_CIFRA_PIN             0x03

#define PULSACION_LARGA                 0x7FFF

#define ESTADO_NORMAL                   0x00
#define ESTADO_RECUP_BAT                0x01

#define TAM_MEMORIA_EEPROM              0x8000

//////////////////////////////////////////////////////////////    
//MACROS
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////    
//CONSTANTES
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////    
//TIPOS DEFINIDOS Y ESTRUCTURAS
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////    
//METODOS PUBLICOS   Definición de funciones o prototipos
//////////////////////////////////////////////////////////////
void MenuOperario (void);
#endif
