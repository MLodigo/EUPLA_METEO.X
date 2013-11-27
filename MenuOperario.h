#include "General.h"


#ifndef MENUOPERARIO_H
    #define MENUOPERARIO_H

 //////////////////////////////////////////////////////////////    
//INCLUDES
//////////////////////////////////////////////////////////////
	
	
//////////////////////////////////////////////////////////////    
//DEFINES
//////////////////////////////////////////////////////////////
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

//#define TAM_MEMORIA_EEPROM              0x8000

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
