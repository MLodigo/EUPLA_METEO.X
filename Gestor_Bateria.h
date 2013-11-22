#ifndef GESTOR_BATERIA_H
    #define GESTOR_BATERIA_H

//////////////////////////////////////////////////////////////    
//INCLUDES
//////////////////////////////////////////////////////////////
#include "General.h"
	
//////////////////////////////////////////////////////////////    
//TIPOS DEFINIDOS Y ESTRUCTURAS
//////////////////////////////////////////////////////////////
typedef enum _NivelBateria
{
    NIVEL_BAJO = 0,
    NIVEL_MEDIO_BAJO,
    NIVEL_MEDIO,
    NIVEL_MEDIO_ALTO,
    NIVEL_ALTO
}NIVEL_BATERIA;

typedef enum _ESTADOSISTEM
{
    NORMAL = 0,
    RECUPERACION,
    INDETERMINADO
}ESTADO_SISTEMA;

//////////////////////////////////////////////////////////////    
//METODOS PUBLICOS   Definición de funciones o prototipos
//////////////////////////////////////////////////////////////
BOOL Si_En_Modo_Recuperacion(void);
NIVEL_BATERIA Comprobacion_Estado_Bateria(void);
BOOL Activar_Modo_Recuperacion(void);
BOOL Desactivar_Modo_Recuperacion(void);

#endif
