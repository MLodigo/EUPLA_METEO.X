#ifndef GESTOREEPROM_H
    #define GESTOREEPROM_H

 //////////////////////////////////////////////////////////////    
//INCLUDES
//////////////////////////////////////////////////////////////
#include "General.h"
	
//////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////
#define DIR_CNT_DESPIERTES              0x0000
#define DIR_DESPIERTES_POR_MEDIDA       0x0002
#define DIR_CNT_MUESTRAS_TOMADAS        0x0004
#define DIR_MUESTRAS_POR_ENVIO_MODEM    0x0006
#define DIR_ESTADO_SISTEMA              0x0008

#define NUM_MAX_MEDIDAS                 8000
#define DIR_BASE_MEDIDAS                0x0300
#define BYTES_POR_MEDIDA                4
#define TAM_MEMORIA_EEPROM              0x8000

//////////////////////////////////////////////////////////////
//TIPOS DEFINIDOS Y ESTRUCTURAS
//////////////////////////////////////////////////////////////
typedef enum _RESPUESTA
{
    NOK = 0,
    OK = 1,
    FALLO = 2
}RESPUESTA;

//////////////////////////////////////////////////////////////    
//METODOS PUBLICOS   Definición de funciones o prototipos
//////////////////////////////////////////////////////////////
BOOL Inicializacion_Modulo_EEPROM();
BOOL Envio_Memoria_UART();
BOOL Lectura_Completa_EEPROM();
BOOL Borrado_Completo_EEPROM();
BOOL Aplicar_Configuracioin_Fabrica();

#endif


