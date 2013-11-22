
#include "Gestor_EEPROM.h"
#include "SPI_EEPROM.h"
#include "Temporizacion.h"
#include "UART.h"

//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PUBLICOS
//********************************************************************************************************************
//********************************************************************************************************************


//Funci�n que habilita el m�dulo SPI del micro, as� como las l�neas que conectan con la memoria EEPROM
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


//Funci�n que realiza el env�o del contenido de la memoria a trav�s de la UART
BOOL Envio_Memoria_UART()
{
    WORD cntDireccion=0;

    //Habilitamos la UART
    UART2_Configura_Abre();
    // TODO Ajustar la direcci�n de inicio y final en la lectura de la memoria para envio por UART
    //Lectura de cada uno de los datos de la memoria y env�o por la UART
    for(cntDireccion=0x300; cntDireccion<0x500; cntDireccion++)
    {
        UART2_Envia_Byte(EEPROM_ReadByte(cntDireccion));
    }

    return TRUE;
}

//Funci�n que realiza una lectura completa de la memoria EEPROM en RAM para operaci�nes de depuraci�n y desarrollo.
BOOL Lectura_Completa_EEPROM()
{
    WORD cntDireccion=0;
    BYTE Memoria[0x400];

    //Lectura Memoria
    //Lectura completa de la memoria (3 segundos)
    for(cntDireccion=0; cntDireccion<0x400; cntDireccion++)
    {
        Memoria[cntDireccion] = EEPROM_ReadByte(cntDireccion);
    }

    return TRUE;
}

//Funci�n que inicializa la memoria EEPROM poniendo todas las posici�nes a 0xFF
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

//Funci�n que inicializa la memoria EEPROM a par�metros de f�brica.
BOOL Aplicar_Configuracioin_Fabrica()
{
    WORD_VAL Dato;
    // TODO Habilitar borrado
    //Borrado_Completo_EEPROM();
    // TODO Cambiar el valor de la frecuencia de muestreo de sensores
    Dato.Val=3; //15; //Toma medidas cada minuto
    EEPROM_WriteByte(Dato.byte.LB, DIR_DESPIERTES_POR_MEDIDA);
    EEPROM_WriteByte(Dato.byte.HB, DIR_DESPIERTES_POR_MEDIDA+1);

    Dato.Val=5;  //Envia los datos v�a modem cada 3 minutos
    EEPROM_WriteByte(Dato.byte.LB, DIR_MUESTRAS_POR_ENVIO_MODEM);
    EEPROM_WriteByte(Dato.byte.HB, DIR_MUESTRAS_POR_ENVIO_MODEM+1);

    //Estado del sistema en modo normal con bater�a a nivel adecuado
    // TODO Ajustar correctamente el estado del sistema
    Dato.Val=1;
    EEPROM_WriteByte(Dato.byte.LB, DIR_ESTADO_SISTEMA);

    return TRUE;
}



