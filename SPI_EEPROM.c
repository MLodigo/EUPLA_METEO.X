/*
    FUNCIONAMIENTO DEL BUS SPI
    ===========================
        El bus SPI posee dos registro, uno de transmisión y otro de recepción. El registro de transmisión del dispositivo
    maestro, está conectado al registro de recepción del dispositivo esclavo, y a su vez, el registro de recepción del
    maestro, está conectado al registro de transmisión del esclavo.
    
        Cada vez que el maestro realiza una transmisión, entra en el registro de recepción un dato, que no es significativo,
    pero que ocupa el registro de recepción. Por tanto tras cada transmisión es necesario hacer una lectura del registro de
    recepción.
    
        Cada vez que el maestro quiere leer un dato, éste tiene que enviar un dummy byte (dato no significativo) y a continuación
    realizar la operación de lectura del buffer de recepción para obtener el dato relevante.
*/

#include "SPI_EEPROM.h"
#include "SPI.h"

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                VARIABLES MIEMBRO
//********************************************************************************************************************    
//********************************************************************************************************************    


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                METODOS PUBLICOS
//********************************************************************************************************************    
//********************************************************************************************************************    

/************************************************************************
* Precondiciones: El módulo SPI debe ser configurado para operar con:
*                 Modo Maestro
*                 8 bits por dato
                  CKP = 1  (Según datasheet memoria)
                  SMP = 1   (Muestreo al final del data output time)
* Descripción: Esta función realiza el setup de las lineas conectadas 
               a la EEPROM.
************************************************************************/
void EEPROM_Inicializacion()
{
    //Ajuste de los triestados de las lineas que conectan con la memoria 
    EEPROM_CS_PORT = 1;       //Chip Select nivel alto (reposo)
    EEPROM_CS_TRIS = 0;       //Chip Select como output
    EEPROM_SCK_TRIS = 0;      //Linea de reloj como output
    EEPROM_SDO_TRIS = 0;      //Linea de datos out como output
    EEPROM_SDI_TRIS = 1;      //Linea de datos in como entrada
    
    //ES NECESARIO GUARDAR UN RETARDO PARA ESTABILIZAR LAS LÍNEAS TRAS ESTA FUNCIÓN
}

/************************************************************************
* Precondiciones: El módulo SPI debe estar configurado para operar con la 
*                 memoria EEPROM                                                                       *
* Descripción: Esta función un dato en la dirección indicada
************************************************************************/
void EEPROM_WriteByte(BYTE dato, WORD direccion)
{
    //Habilitar escritura
    EEPROM_WriteEnable();
    
    //Macro para bajar la línea CS (Chip Select)
    mEEPROM_CS_Low();

    //Envío del comando de escritura
    SPI_EscribeDato(EEPROM_CMD_WRITE);
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();
    
    //Envío de la parte alta de la dirección
    SPI_EscribeDato(HighByte_Word(direccion));
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Envío de la parte baja de la dirección
    SPI_EscribeDato(LowByte_Word(direccion));
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Envío del dato
    SPI_EscribeDato(dato);
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Macro para levantar la línea de CS
    mEEPROM_CS_High();

    //Se puede leer el registro STATUS en mitad de un ciclo de escritura
    //Espera que se complete la escritura (WIP-> Write in progress)
    while(EEPROM_ReadStatus().Bits.WIP);
}


/************************************************************************
* Precondiciones: El módulo SPI debe estar configurado para operar con la 
*                 memoria EEPROM
*
* Descripción: Esta función lee un dato de la dirección indicada
************************************************************************/
BYTE EEPROM_ReadByte(WORD direccion){
  BYTE Temp;
  
    //Macro para bajar la línea CS (Chip Select)
    mEEPROM_CS_Low();

    //Envío del comando de lectura
    SPI_EscribeDato(EEPROM_CMD_READ);
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Envío de la parte alta de la dirección
    SPI_EscribeDato(HighByte_Word(direccion));
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Envío de la parte baja de la dirección
    SPI_EscribeDato(LowByte_Word(direccion));
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Envío del dummy byte para leer el dato
    SPI_EscribeDato(0);
    SPI_TransmisionFinalizada();
    Temp = mSPI_LeeSPIBUF();

    //Macro para levantar la línea de CS
    mEEPROM_CS_High();
    
    return Temp;
}

/************************************************************************
* Preconditions: El módulo SPI de estar configurado para trabajar con la
                 memoria.
* Descripción: Esta función habilita el poder escribir en la EEPROM
*              Debe ser llamada antes de cada comando de escritura.
************************************************************************/
void EEPROM_WriteEnable(){
    
    //Macro para bajar la línea CS (Chip Select)
    mEEPROM_CS_Low();
    
    //Envío del comando de habilitación de escritura
    SPI_EscribeDato(EEPROM_CMD_WREN);
    //Esperamos la terminación del envío
    SPI_TransmisionFinalizada();
    //Limpiamos el dato recibido (Tras realizar la transmisión. Por construcción del bus SPI)
    mSPI_LeeSPIBUF();
    
    //Macro para levantar la línea de CS
    mEEPROM_CS_High();
}


/************************************************************************
* Precondiciones: El módulo SPI debe estar configurado para operar con 
                  la EEPROM
* Descripción: Esta función realiza la lectura del registro de estado de
*              la memoria EEPROM
************************************************************************/
union _EEPROMStatus_ EEPROM_ReadStatus(){
   char Temp;

    //Macro para bajar la línea CS (Chip Select)
    mEEPROM_CS_Low();
    
    //Envío del comando de lectura del registro STATUS
    SPI_EscribeDato(EEPROM_CMD_RDSR);
    //Espera final de transmisión
    SPI_TransmisionFinalizada();
    //Limpiamos el dato recibido (Tras realizar la transmisión. Por construcción del bus SPI)
    mSPI_LeeSPIBUF();
    
    //Escribimos un Dummy byte para poder realizar la lectura del registro. Nuevamente es la construcción del bus SPI.
    SPI_EscribeDato(0);
    SPI_TransmisionFinalizada();
    Temp = mSPI_LeeSPIBUF();
    
    //Macro para levantar la línea de CS
    mEEPROM_CS_High();

    return (union _EEPROMStatus_)Temp;
}
