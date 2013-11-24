
#include "Switches.h"
#include "General.h"


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                VARIABLES MIEMBRO
//********************************************************************************************************************    
//********************************************************************************************************************    

//Variable donde se guarda el estado actual de los switches
PRIVATE VAL_SWITCHES EstadoSwithes;

//Variables contador utilizadas para asegurar la pulsación de un switch (Técnica de debounce)
#define	DEBOUCE_MAX 	20
unsigned char Cnt_SW3 = 0;
unsigned char Cnt_SW4 = 0;
unsigned char Cnt_SW5 = 0;
unsigned char Cnt_SW6 = 0;


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                MÉTODOS PRIVADOS
//********************************************************************************************************************    
//********************************************************************************************************************    




//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                METODOS PÚBLICOS
//********************************************************************************************************************    
//********************************************************************************************************************    

//********************************************************************************************************************
//Función que incializa el estado de los switches a reposo.
//********************************************************************************************************************
void Inicializa_Switches(void)
{
    //Inicialmente los pulsadores estan en reposo.
    EstadoSwithes.bits.SW3 = 0;
    EstadoSwithes.bits.SW4 = 0;
    EstadoSwithes.bits.SW5 = 0;
    EstadoSwithes.bits.SW6 = 0;
    
    //Contadores de debounce en posición.
    Cnt_SW3 = 128;
    Cnt_SW4 = 128;
    Cnt_SW5 = 128;
    Cnt_SW6 = 128;
    
    //Configuración del triestado de las líneas del micro asociadas a los Switches como Inputs. 
    SWITCH_3_TRIS = 1;
    SWITCH_4_TRIS = 1;
    SWITCH_5_TRIS = 1;
    SWITCH_6_TRIS = 1;
}

//********************************************************************************************************************
//Función que consulta el estado actual de un switch (0-> Resposo 1-> Activado)
//Distribución:
//   Bit 0 -> Switch 3
//   Bit 1 -> Switch 4
//   Bit 2 -> Switch 5
//   Bit 3 -> Switch 6
//
//Para consultar el estado de un switch, rotamos hasta la posicón del bit 0 el switch en cuestión, y luego hacemos una máscara con 0x01 .
//********************************************************************************************************************
BOOL Switch_ON(ID_SWITCHES idSwitch)
{
    return (BOOL)((EstadoSwithes.Val >> idSwitch) & (0x01));
}

//********************************************************************************************************************
//Función que realiza una lectura de las líneas del micro donde se encuentran conectados los switches, y guarda
//el estado de cada uno de ellos en la variable miembro EstadoSwitches.
//Se establece una técnica de histéresis, de manera que es necesario alcanzar un nivel (DEBOUCE_MAX) por encima
//o por debajo del valor 128 para determinar que un pulsador está activado o desactivado.
//********************************************************************************************************************
void Actualiza_Estado_Switches(void)
{
    //Comprobamos el estado del Pulsador SW3
    if(!SWITCH_3_IO)
    {
        Cnt_SW3++;
        if(Cnt_SW3 > (128 + DEBOUCE_MAX))
        {
            Cnt_SW3 = (128 + DEBOUCE_MAX); //Lo mantenemos en el límite superior sin superarlo
            EstadoSwithes.bits.SW3 = 1;    //SW Pulsado
        }
    }
    else
    {
        Cnt_SW3--;
        if(Cnt_SW3 < (128 - DEBOUCE_MAX))
        {
            Cnt_SW3 = (128 - DEBOUCE_MAX); //Lo mantenemos en el límite inferior sin superarlo
            EstadoSwithes.bits.SW3 = 0;    //SW Reposo
        }
    }
    
    //Comprobamos el estado del Pulsador SW4
    if(!SWITCH_4_IO)
    {
        Cnt_SW4++;
        if(Cnt_SW4 > (128 + DEBOUCE_MAX))
        {
            Cnt_SW4 = (128 + DEBOUCE_MAX); //Lo mantenemos en el límite superior sin superarlo
            EstadoSwithes.bits.SW4 = 1;    //SW Pulsado
        }
    }
    else
    {
        Cnt_SW4--;
        if(Cnt_SW4 < (128 - DEBOUCE_MAX))
        {
            Cnt_SW4 = (128 - DEBOUCE_MAX); //Lo mantenemos en el límite inferior sin superarlo
            EstadoSwithes.bits.SW4 = 0;    //SW Reposo
        }
    }
    
    //Comprobamos el estado del Pulsador SW5
    if(!SWITCH_5_IO)
    {
        Cnt_SW5++;
        if(Cnt_SW5 > (128 + DEBOUCE_MAX))
        {
            Cnt_SW5 = (128 + DEBOUCE_MAX); //Lo mantenemos en el límite superior sin superarlo
            EstadoSwithes.bits.SW5 = 1;    //SW Pulsado
        }
    }
    else
    {
        Cnt_SW5--;
        if(Cnt_SW5 < (128 - DEBOUCE_MAX))
        {
            Cnt_SW5 = (128 - DEBOUCE_MAX); //Lo mantenemos en el límite inferior sin superarlo
            EstadoSwithes.bits.SW5 = 0;    //SW Reposo
        }
    }
    
    //Comprobamos el estado del Pulsador SW6
    if(!SWITCH_6_IO)
    {
        Cnt_SW6++;
        if(Cnt_SW6 > (128 + DEBOUCE_MAX))
        {
            Cnt_SW6 = (128 + DEBOUCE_MAX); //Lo mantenemos en el límite superior sin superarlo
            EstadoSwithes.bits.SW6 = 1;    //SW Pulsado
        }
    }
    else
    {
        Cnt_SW6--;
        if(Cnt_SW6 < (128 - DEBOUCE_MAX))
        {
            Cnt_SW6 = (128 - DEBOUCE_MAX); //Lo mantenemos en el límite inferior sin superarlo
            EstadoSwithes.bits.SW6 = 0;    //SW Reposo
        }
    }
}