
#include "Gestor_EEPROM.h"
#include "SPI_EEPROM.h"
#include "Temporizacion.h"
#include "UART.h"

//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PUBLICOS
//********************************************************************************************************************
//********************************************************************************************************************

//********************************************************************************************************************
//Funci�n que configura el m�dulo SPI del micro, as� como las l�neas que lo conectan con la memoria EEPROM externa.
//********************************************************************************************************************
BOOL Inicializacion_Modulo_EEPROM()
{
    //Configuraci�n m�dulo SPI del micro para comunicar con la EEPROM
    SPI_Inicializacion();
    //Configuraci�n de las l�neas de puerto que conectan con la EEPROM
    EEPROM_Inicializacion();
    //Retardo para asegurarnos la inicializaci�n (IMPORTANTE)
    Retardo(50);

    return TRUE;
}

//********************************************************************************************************************
//Funci�n que realiza el env�o del contenido de la memoria a trav�s de la UART para funciones de depuraci�n.
//********************************************************************************************************************
BOOL Envio_Memoria_UART()
{
    WORD cntDireccion=0;

    //Habilitamos la UART
    UART2_Configura_Abre();
    // TODO Ajustar la direcci�n de inicio y final en la lectura de la memoria para envio por UART
    //Lectura de cada uno de los datos de la memoria y env�o por la UART
    for(cntDireccion=0; cntDireccion<0x500; cntDireccion++)
    {
        UART2_Envia_Byte(EEPROM_ReadByte(cntDireccion));
        //TODO: Enviar en formato "0x300 -- FF" y un CRLF as� se visualizar� m�s claramente.
    }

    return TRUE;
}

//********************************************************************************************************************
//Funci�n que realiza una lectura completa de la memoria EEPROM en RAM para operaci�nes de depuraci�n y desarrollo.
//********************************************************************************************************************
BOOL Lectura_Completa_EEPROM()
{
    WORD cntDireccion=0;
    BYTE Memoria[0x400];

    //Lectura Memoria. Lectura completa de la memoria (3 segundos).
    for(cntDireccion=0; cntDireccion<0x400; cntDireccion++)
    {
        Memoria[cntDireccion] = EEPROM_ReadByte(cntDireccion);
    }

    return TRUE;
}

//********************************************************************************************************************
//Funci�n que inicializa la memoria EEPROM poniendo todas las posici�nes a 0xFF.
//********************************************************************************************************************
BOOL Borrado_Completo_EEPROM()
{
    WORD cntDireccion=0;

    //Escritura completa de la memoria (2 minutos)
    for(cntDireccion=0; cntDireccion<TAM_MEMORIA_EEPROM; cntDireccion++)
    {
        EEPROM_WriteByte(0xFF, cntDireccion);
    }

    return TRUE;
}
