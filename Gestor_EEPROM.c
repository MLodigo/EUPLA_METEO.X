
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
//Formato: <0x>Dirección<espacio>Dato<CRLF>
//********************************************************************************************************************
BOOL Envio_Memoria_UART()
{
    WORD cntDireccion=0;
    BYTE Dato = 0;
    BYTE Cifra = 0;

    //Habilitamos la UART
    UART2_Configura_Abre();

    //Lectura de cada uno de los datos de la memoria y envío por la UART.
    //Solamente se envía la zona de configuración y las primeras muestras.
    for(cntDireccion=0; cntDireccion<0x400; cntDireccion++)
    {
        //Envio de dirección///////////////////////
        UART2_Envia_Byte('0');
        UART2_Envia_Byte('x');
        //Dirección: Cifra 1
        Cifra = (BYTE)((cntDireccion & 0xF000)>>12);
        if(Cifra<0x0A){Cifra += 0x30;}
        else{Cifra += 0x37;}
        UART2_Envia_Byte(Cifra);
        //Dirección: Cifra 2
        Cifra = (BYTE)((cntDireccion & 0x0F00)>>8);
        if(Cifra<0x0A){Cifra += 0x30;}
        else{Cifra += 0x37;}
        UART2_Envia_Byte(Cifra);
        //Dirección: Cifra 3
        Cifra = (BYTE)((cntDireccion & 0x00F0)>>4);
        if(Cifra<0x0A){Cifra += 0x30;}
        else{Cifra += 0x37;}
        UART2_Envia_Byte(Cifra);
        //Dirección: Cifra 4
        Cifra = (BYTE)((cntDireccion & 0x000F));
        if(Cifra<0x0A){Cifra += 0x30;}
        else{Cifra += 0x37;}
        UART2_Envia_Byte(Cifra);

        //Espacio//////////////////////////////////
        UART2_Envia_Byte(' ');
        
        //Envío del dato//////////////////////////
        Dato = EEPROM_ReadByte(cntDireccion);
        //Dato: Cifra 1
        Cifra = (BYTE)((Dato & 0xF0)>>4);
        if(Cifra<0x0A){Cifra += 0x30;}
        else{Cifra += 0x37;}
        UART2_Envia_Byte(Cifra);
        //Dato: Cifra 2
        Cifra = (BYTE)((Dato & 0x0F));
        if(Cifra<0x0A){Cifra += 0x30;}
        else{Cifra += 0x37;}
        UART2_Envia_Byte(Cifra);

        //Salto de línea///////////////////////////
        UART2_Envia_Byte('\r');
        UART2_Envia_Byte('\n');
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
    for(cntDireccion=0; cntDireccion<TAM_MEMORIA_EEPROM; cntDireccion++)
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
