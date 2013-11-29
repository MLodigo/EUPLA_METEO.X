
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
//Formato: <0x>Direcci�n<espacio>Dato<CRLF>
//********************************************************************************************************************
BOOL Envio_Memoria_UART()
{
    WORD cntDireccion=0;
    BYTE Dato = 0;
    BYTE Cifra = 0;

    //Habilitamos la UART
    UART2_Configura_Abre();

    //Lectura de cada uno de los datos de la memoria y env�o por la UART.
    //Solamente se env�a la zona de configuraci�n y las primeras muestras.
    for(cntDireccion=0; cntDireccion<0x400; cntDireccion++)
    {
        //Envio de direcci�n///////////////////////
        UART2_Envia_Byte('0');
        UART2_Envia_Byte('x');
        //Direcci�n: Cifra 1
        Cifra = (BYTE)((cntDireccion & 0xF000)>>12);
        if(Cifra<0x0A){Cifra += 0x30;}
        else{Cifra += 0x37;}
        UART2_Envia_Byte(Cifra);
        //Direcci�n: Cifra 2
        Cifra = (BYTE)((cntDireccion & 0x0F00)>>8);
        if(Cifra<0x0A){Cifra += 0x30;}
        else{Cifra += 0x37;}
        UART2_Envia_Byte(Cifra);
        //Direcci�n: Cifra 3
        Cifra = (BYTE)((cntDireccion & 0x00F0)>>4);
        if(Cifra<0x0A){Cifra += 0x30;}
        else{Cifra += 0x37;}
        UART2_Envia_Byte(Cifra);
        //Direcci�n: Cifra 4
        Cifra = (BYTE)((cntDireccion & 0x000F));
        if(Cifra<0x0A){Cifra += 0x30;}
        else{Cifra += 0x37;}
        UART2_Envia_Byte(Cifra);

        //Espacio//////////////////////////////////
        UART2_Envia_Byte(' ');
        
        //Env�o del dato//////////////////////////
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

        //Salto de l�nea///////////////////////////
        UART2_Envia_Byte('\r');
        UART2_Envia_Byte('\n');
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
    for(cntDireccion=0; cntDireccion<TAM_MEMORIA_EEPROM; cntDireccion++)
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
