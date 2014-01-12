#include "Gestor_Bateria.h"
#include "General.h"
#include "Gestor_EEPROM.h"
#include "SPI_EEPROM.h"
#include "ADC.h"


//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PUBLICOS
//********************************************************************************************************************
//********************************************************************************************************************

//********************************************************************************************************************
//Funci�n que comprueba si el sistema est� actualmente en modo de recuperaci�n de la bater�a.
//********************************************************************************************************************
BOOL Modo_Recuperacion()
{
    ESTADO_SISTEMA StatusSistema = INDETERMINADO;
    BYTE Reintentos = 0;

    //Lectura del contador de despiertes
    do
    {
        StatusSistema = (ESTADO_SISTEMA)EEPROM_ReadByte(DIR_ESTADO_SISTEMA);
        Reintentos++;
    }while((StatusSistema==0xFF)&&(Reintentos<3));

    if(Reintentos==3){return FALLO;}


    //Valoraci�n final
    if(StatusSistema == NORMAL)
    {
        return NOK;
    }
    else if(StatusSistema == RECUPERACION)
    {
        return OK;
    }
    else
    {
        return ERROR;
    }

}

//********************************************************************************************************************
//Funci�n que comprueba el nivel de la bater�a leyendo el sensor asociado mediante el conversor ADC.
//********************************************************************************************************************
NIVEL_BATERIA Comprobacion_Estado_Bateria()
{
    SENSORES Muestra;
    BYTE Reintentos = 0;

    //Inicializamos variable Muestra
    Muestra.Nivel_Bateria = 0xFFFF;

    //Lectura Sensores
    do
    {
        ADC_Configura_Inicia();
        while(!ADC_HayNuevaMedida());
        Muestra = ADC_Lectura_Sensores();
        ADC_Detiene();
        Reintentos++;
    }
    while((Muestra.Nivel_Bateria==0xFFFF)&&(Reintentos<3));

    if(Muestra.Nivel_Bateria<=UMBRAL_BAJO)
    {
        return NIVEL_BAJO; 
    }
    else if((Muestra.Nivel_Bateria>UMBRAL_BAJO)&&(Muestra.Nivel_Bateria<=UMBRAL_MEDIO_BAJO))
    {
        return NIVEL_MEDIO_BAJO;
    }
    else if((Muestra.Nivel_Bateria>UMBRAL_MEDIO_BAJO)&&(Muestra.Nivel_Bateria<=UMBRAL_MEDIO))
    {
        return NIVEL_MEDIO;
    }
    else if((Muestra.Nivel_Bateria>UMBRAL_MEDIO)&&(Muestra.Nivel_Bateria<=UMBRAL_MEDIO_ALTO))
    {
        return NIVEL_MEDIO_ALTO;
    }
    else if(Muestra.Nivel_Bateria>UMBRAL_ALTO)
    {
        return NIVEL_ALTO;
    } 
}

//********************************************************************************************************************
//Funci�n que pone al sistema en modo de recuperaci�n de la bater�a.
//Acciones: Desconexi�n del modem.
//********************************************************************************************************************
BOOL Activar_Modo_Recuperacion()
{
    BYTE Reintentos = 0;

    //Actualizaci�n en memoria del nuevo estado del sistema: Recuperaci�n de bater�a.
    do
    {
        EEPROM_WriteByte(RECUPERACION, DIR_ESTADO_SISTEMA);
        Reintentos++;
    }
    while((EEPROM_ReadByte(DIR_ESTADO_SISTEMA)!=RECUPERACION)&&(Reintentos<3));

    //Comprobaci�n final..
    if(Reintentos==3)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//********************************************************************************************************************
//Funci�n que pone al sistema del modo normal de funcionamiento.
//Acciones: Conexi�n del modem.
//********************************************************************************************************************
BOOL Desactivar_Modo_Recuperacion()
{
    BYTE Reintentos = 0;

    //Actualizaci�n en memoria del nuevo estado del sistema: Modo Normal.
    do
    {
        EEPROM_WriteByte(NORMAL, DIR_ESTADO_SISTEMA);
        Reintentos++;
    }
    while((EEPROM_ReadByte(DIR_ESTADO_SISTEMA)!=NORMAL)&&(Reintentos<3));

    //Comprobaci�n final..
    if(Reintentos==3)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//********************************************************************************************************************
//Funci�n que devuelve una cadena para mostrarse en una l�nea del display LCD
//indicando el nivel actual de la bater�a
//********************************************************************************************************************
unsigned char* Porcentaje_Nivel_Bateria(NIVEL_BATERIA nivel)
{
    if(nivel == NIVEL_BAJO)
    {
        return (unsigned char*) "Bateria: 20%    ";
    }
    else if(nivel == NIVEL_MEDIO_BAJO)
    {
        return (unsigned char*) "Bateria: 40%    ";
    }
    else if(nivel == NIVEL_MEDIO)
    {
        return (unsigned char*) "Bateria: 60%    ";
    }
    else if(nivel == NIVEL_MEDIO_ALTO)
    {
        return (unsigned char*) "Bateria: 80%    ";
    }
    else if(nivel == NIVEL_ALTO)
    {
        return (unsigned char*) "Bateria: 95%    ";
    }
    else
    {
        return (unsigned char*) "Bateria: 100%   ";
    }
}
