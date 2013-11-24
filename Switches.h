
#ifndef SWITCHES_H
    #define SWITCHES_H

//////////////////////////////////////////////////////////////    
//INCLUDES
//////////////////////////////////////////////////////////////
#include "General.h"
#include <p24FJ128GA010.h>

//////////////////////////////////////////////////////////////    
//DEFINES
//////////////////////////////////////////////////////////////

#define SWITCH_3_IO   PORTDbits.RD6
#define SWITCH_4_IO   PORTDbits.RD13
#define SWITCH_6_IO   PORTDbits.RD7
#define SWITCH_5_IO   PORTAbits.RA7

#define SWITCH_3_TRIS	TRISDbits.TRISD6
#define SWITCH_4_TRIS	TRISDbits.TRISD13
#define SWITCH_5_TRIS	TRISAbits.TRISA7
#define SWITCH_6_TRIS	TRISDbits.TRISD7

//////////////////////////////////////////////////////////////    
//MACROS
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////    
//CONSTANTES
//////////////////////////////////////////////////////////////

//Tipo enumerado (Constantes) para hacer referencia a los switches sobre el byte que guarda el valor global (VAL_SWITCHES)
typedef enum _EnumSwitches
{
    SW3 = 0,
    SW4,
    SW5,
    SW6
}ID_SWITCHES;

//////////////////////////////////////////////////////////////    
//TIPOS DEFINIDOS Y ESTRUCTURAS
//////////////////////////////////////////////////////////////

//Tipo definido para tener acceso tipo bit al dato donde se guarda la información de los switches
typedef struct
{
    BYTE    SW3:     1;
    BYTE    SW4:     1;
    BYTE    SW5:     1;
    BYTE    SW6:     1;
}SWITCHES;

//Unión para manejar un la información de los switches como valor, o por bits individualmente
typedef union _SWITCHES
{
    SWITCHES bits;
    BYTE Val;
} VAL_SWITCHES;

//////////////////////////////////////////////////////////////    
//METODOS PUBLICOS   Definición de funciones o prototipos
//////////////////////////////////////////////////////////////
void Inicializa_Switches(void);
void Actualiza_Estado_Switches(void);
BOOL Switch_ON(ID_SWITCHES idSwitch);

#endif