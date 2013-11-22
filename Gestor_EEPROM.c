
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

//********************************************************************************************************************
//Función que inicializa la memoria EEPROM a parámetros de fábrica.
// -Se realiza un borrado completo de la memoria.
// -Muestreo de sensores:           1 minuto.
// -Envío de mediciones vía modem:  3 minutos.
// -Estado del sistema:             Modo Normal.
//********************************************************************************************************************
BOOL Aplicar_Configuracion_Fabrica()
{
    WORD_VAL Dato;
    // TODO Habilitar borrado
    //Borrado_Completo_EEPROM();
    // TODO Cambiar el valor de la frecuencia de muestreo de sensores
    Dato.Val=3; //15; //Toma medidas cada minuto
    EEPROM_WriteByte(Dato.byte.LB, DIR_DESPIERTES_POR_MEDIDA);
    EEPROM_WriteByte(Dato.byte.HB, DIR_DESPIERTES_POR_MEDIDA+1);

    Dato.Val=5;  //Envia los datos vía modem cada 3 minutos
    EEPROM_WriteByte(Dato.byte.LB, DIR_MUESTRAS_POR_ENVIO_MODEM);
    EEPROM_WriteByte(Dato.byte.HB, DIR_MUESTRAS_POR_ENVIO_MODEM+1);

    //Estado del sistema en modo normal con batería a nivel adecuado
    // TODO Ajustar correctamente el estado del sistema
    Dato.Val=1;
    EEPROM_WriteByte(Dato.byte.LB, DIR_ESTADO_SISTEMA);

    return TRUE;
}