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
#define DIR_ALARMA_SMS                  0x0009
#define DIR_TLF_ALARMA                  0x000A
#define DIR_PIN_SIM_MODEM               0x0013

#define NUM_MAX_MEDIDAS                 2461        //32000 / 13 = 2461 * BYTES_POR_MEDIDA = 31993 bytes
#define DIR_BASE_MEDIDAS                0x0300      //TAM_MEMORIA_EEPROM - DIR_BASE_MEDIDAS = 32000 bytes para guardar medidas
#define BYTES_POR_MEDIDA                13          //DDMMYYHHMMTTTTPPPPVVVVBBBB --> Fecha,Hora,Temp,Pluv,Vel.Air,Batt.
#define TAM_MEMORIA_EEPROM              32768       //25LC256 -- 256 Kbits -- 32 Kbytes -- 32768 bytes

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

#endif


