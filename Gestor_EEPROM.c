
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
//Función que configura el módulo SPI del micro, así como las líneas que lo conectan con la memoria EEPROM externa.
//********************************************************************************************************************
BOOL Inicializacion_Modulo_EEPROM()
{
    //Configuración módulo SPI del micro para comunicar con la EEPROM
    SPI_Inicializacion();
    //Configuración de las líneas de puerto que conectan con la EEPROM
    EEPROM_Inicializacion();
    //Retardo para asegurarnos la inicialización (IMPORTANTE)
    Retardo(50);

    return TRUE;
}

//********************************************************************************************************************
//Función que realiza el envío del contenido de la memoria a través de la UART para funciones de depuración.
//********************************************************************************************************************
BOOL Envio_Memoria_UART()
{
    WORD cntDireccion=0;

    //Habilitamos la UART
    UART2_Configura_Abre();
    // TODO Ajustar la dirección de inicio y final en la lectura de la memoria para envio por UART
    //Lectura de cada uno de los datos de la memoria y envío por la UART
    for(cntDireccion=0; cntDireccion<0x500; cntDireccion++)
    {
        UART2_Envia_Byte(EEPROM_ReadByte(cntDireccion));
        //TODO: Enviar en formato "0x300 -- FF" y un CRLF así se visualizará más claramente.
    }

    return TRUE;
}

//********************************************************************************************************************
//Función que realiza una lectura completa de la memoria EEPROM en RAM para operaciónes de depuración y desarrollo.
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
//Función que inicializa la memoria EEPROM poniendo todas las posiciónes a 0xFF.
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
