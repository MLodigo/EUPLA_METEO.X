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

//********************************************************************************************************************
//Funci�n que comprueba si el sistema est� actualmente en modo de recuperaci�n de la bater�a.
//********************************************************************************************************************
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
        //Apagado de la alimentaci�n del modem
        MODEM_OFF;
        return TRUE;
    }
}

//********************************************************************************************************************
//Funci�n que pne al sistema del modo normal de funcionamiento.
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
        //Conexi�n de la alimentaci�n del modem
        MODEM_ON;
        return TRUE;
    }
}
