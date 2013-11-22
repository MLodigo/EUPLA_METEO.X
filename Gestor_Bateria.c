#include "Gestor_Bateria.h"
#include "General.h"
#include "Gestor_EEPROM.h"
#include "SPI_EEPROM.h"
#include "ADC.h"
#include "Rele.h"


//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PUBLICOS
//********************************************************************************************************************
//********************************************************************************************************************

//Función que comprueba si el sistema está actualmente en modo de recuperación
BOOL Si_En_Modo_Recuperacion()
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


    //Valoración final
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

//Función que comprueba el nivel de la batería
NIVEL_BATERIA Comprobacion_Estado_Bateria()
{
    SENSORES Muestra;
    BYTE Reintentos = 0;

    //Inicializamos variable Muestra
    Muestra.Nivel_Bateria = 0xFFFFFFFF;

    //Lectura Sensores
    do
    {
        ADC_Configura_Inicia();
        while(!ADC_HayNuevaMedida());
        Muestra = ADC_Lectura_Sensores();
        ADC_Detiene();
        Reintentos++;
    }
    while((Muestra.Nivel_Bateria==0xFFFFFFFF)&&(Reintentos<3));

    if(Muestra.Nivel_Bateria<=1000)
    {
        return NIVEL_BAJO; //TODO Modificar constantes de tramos de niveles BAJO, MEDIO, ALTO..
    }
    else if((Muestra.Nivel_Bateria>1000)&&(Muestra.Nivel_Bateria<=1500))
    {
        return NIVEL_MEDIO_BAJO;
    }
    else if((Muestra.Nivel_Bateria>1500)&&(Muestra.Nivel_Bateria<=2000))
    {
        return NIVEL_MEDIO;
    }
    else if((Muestra.Nivel_Bateria>2000)&&(Muestra.Nivel_Bateria<=3000))
    {
        return NIVEL_MEDIO_ALTO;
    }
    else if(Muestra.Nivel_Bateria>3000)
    {
        return NIVEL_ALTO;
    } 
}

//Función que pone al sistema en modo de recuperación de la batería
BOOL Activar_Modo_Recuperacion()
{
    BYTE Reintentos = 0;

    MODEM_OFF;

    do
    {
        EEPROM_WriteByte(RECUPERACION, DIR_ESTADO_SISTEMA);
        Reintentos++;
    }
    while((EEPROM_ReadByte(DIR_ESTADO_SISTEMA)!=RECUPERACION)&&(Reintentos<3));

    //Comprobación final..
    if(Reintentos==3)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//Función que saca al sistema del modo de recuperación de la bateria
BOOL Desactivar_Modo_Recuperacion()
{
    BYTE Reintentos = 0;

    MODEM_ON;

    do
    {
        EEPROM_WriteByte(NORMAL, DIR_ESTADO_SISTEMA);
        Reintentos++;
    }
    while((EEPROM_ReadByte(DIR_ESTADO_SISTEMA)!=NORMAL)&&(Reintentos<3));

    //Comprobación final..
    if(Reintentos==3)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


