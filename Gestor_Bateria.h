#ifndef GESTOR_BATERIA_H
    #define GESTOR_BATERIA_H

//////////////////////////////////////////////////////////////    
//INCLUDES
//////////////////////////////////////////////////////////////
#include "General.h"
	
//////////////////////////////////////////////////////////////    
//TIPOS DEFINIDOS Y ESTRUCTURAS
//////////////////////////////////////////////////////////////
#define UMBRAL_BAJO         20
#define UMBRAL_MEDIO_BAJO   40
#define UMBRAL_MEDIO        60
#define UMBRAL_MEDIO_ALTO   80
#define UMBRAL_ALTO         95

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
BOOL Modo_Recuperacion(void);
NIVEL_BATERIA Comprobacion_Estado_Bateria(void);
BOOL Activar_Modo_Recuperacion(void);
BOOL Desactivar_Modo_Recuperacion(void);
unsigned char* Porcentaje_Nivel_Bateria(NIVEL_BATERIA);

#endif
