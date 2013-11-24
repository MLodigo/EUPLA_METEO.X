/*
    FUNCIONAMIENTO DEL BUS SPI
    ===========================
        El bus SPI posee dos registro, uno de transmisi�n y otro de recepci�n. El registro de transmisi�n del dispositivo
    maestro, est� conectado al registro de recepci�n del dispositivo esclavo, y a su vez, el registro de recepci�n del
    maestro, est� conectado al registro de transmisi�n del esclavo.
    
        Cada vez que el maestro realiza una transmisi�n, entra en el registro de recepci�n un dato, que no es significativo,
    pero que ocupa el registro de recepci�n. Por tanto tras cada transmisi�n es necesario hacer una lectura del registro de
    recepci�n.
    
        Cada vez que el maestro quiere leer un dato, �ste tiene que enviar un dummy byte (dato no significativo) y a continuaci�n
    realizar la operaci�n de lectura del buffer de recepci�n para obtener el dato relevante.
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
* Precondiciones: El m�dulo SPI debe ser configurado para operar con:
*                 Modo Maestro
*                 8 bits por dato
                  CKP = 1  (Seg�n datasheet memoria)
                  SMP = 1   (Muestreo al final del data output time)
* Descripci�n: Esta funci�n realiza el setup de las lineas conectadas 
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
    
    //ES NECESARIO GUARDAR UN RETARDO PARA ESTABILIZAR LAS L�NEAS TRAS ESTA FUNCI�N
}

/************************************************************************
* Precondiciones: El m�dulo SPI debe estar configurado para operar con la 
*                 memoria EEPROM                                                                       *
* Descripci�n: Esta funci�n un dato en la direcci�n indicada
************************************************************************/
void EEPROM_WriteByte(BYTE dato, WORD direccion)
{
    //Habilitar escritura
    EEPROM_WriteEnable();
    
    //Macro para bajar la l�nea CS (Chip Select)
    mEEPROM_CS_Low();

    //Env�o del comando de escritura
    SPI_EscribeDato(EEPROM_CMD_WRITE);
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();
    
    //Env�o de la parte alta de la direcci�n
    SPI_EscribeDato(HighByte_Word(direccion));
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Env�o de la parte baja de la direcci�n
    SPI_EscribeDato(LowByte_Word(direccion));
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Env�o del dato
    SPI_EscribeDato(dato);
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Macro para levantar la l�nea de CS
    mEEPROM_CS_High();

    //Se puede leer el registro STATUS en mitad de un ciclo de escritura
    //Espera que se complete la escritura (WIP-> Write in progress)
    while(EEPROM_ReadStatus().Bits.WIP);
}


/************************************************************************
* Precondiciones: El m�dulo SPI debe estar configurado para operar con la 
*                 memoria EEPROM
*
* Descripci�n: Esta funci�n lee un dato de la direcci�n indicada
************************************************************************/
BYTE EEPROM_ReadByte(WORD direccion){
  BYTE Temp;
  
    //Macro para bajar la l�nea CS (Chip Select)
    mEEPROM_CS_Low();

    //Env�o del comando de lectura
    SPI_EscribeDato(EEPROM_CMD_READ);
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Env�o de la parte alta de la direcci�n
    SPI_EscribeDato(HighByte_Word(direccion));
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Env�o de la parte baja de la direcci�n
    SPI_EscribeDato(LowByte_Word(direccion));
    SPI_TransmisionFinalizada();
    mSPI_LeeSPIBUF();

    //Env�o del dummy byte para leer el dato
    SPI_EscribeDato(0);
    SPI_TransmisionFinalizada();
    Temp = mSPI_LeeSPIBUF();

    //Macro para levantar la l�nea de CS
    mEEPROM_CS_High();
    
    return Temp;
}

/************************************************************************
* Preconditions: El m�dulo SPI de estar configurado para trabajar con la
                 memoria.
* Descripci�n: Esta funci�n habilita el poder escribir en la EEPROM
*              Debe ser llamada antes de cada comando de escritura.
************************************************************************/
void EEPROM_WriteEnable(){
    
    //Macro para bajar la l�nea CS (Chip Select)
    mEEPROM_CS_Low();
    
    //Env�o del comando de habilitaci�n de escritura
    SPI_EscribeDato(EEPROM_CMD_WREN);
    //Esperamos la terminaci�n del env�o
    SPI_TransmisionFinalizada();
    //Limpiamos el dato recibido (Tras realizar la transmisi�n. Por construcci�n del bus SPI)
    mSPI_LeeSPIBUF();
    
    //Macro para levantar la l�nea de CS
    mEEPROM_CS_High();
}


/************************************************************************
* Precondiciones: El m�dulo SPI debe estar configurado para operar con 
                  la EEPROM
* Descripci�n: Esta funci�n realiza la lectura del registro de estado de
*              la memoria EEPROM
************************************************************************/
union _EEPROMStatus_ EEPROM_ReadStatus(){
   char Temp;

    //Macro para bajar la l�nea CS (Chip Select)
    mEEPROM_CS_Low();
    
    //Env�o del comando de lectura del registro STATUS
    SPI_EscribeDato(EEPROM_CMD_RDSR);
    //Espera final de transmisi�n
    SPI_TransmisionFinalizada();
    //Limpiamos el dato recibido (Tras realizar la transmisi�n. Por construcci�n del bus SPI)
    mSPI_LeeSPIBUF();
    
    //Escribimos un Dummy byte para poder realizar la lectura del registro. Nuevamente es la construcci�n del bus SPI.
    SPI_EscribeDato(0);
    SPI_TransmisionFinalizada();
    Temp = mSPI_LeeSPIBUF();
    
    //Macro para levantar la l�nea de CS
    mEEPROM_CS_High();

    return (union _EEPROMStatus_)Temp;
}