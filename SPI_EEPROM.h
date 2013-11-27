#ifndef SPI_EEPROM_H
    #define SPI_EEPROM_H

//////////////////////////////////////////////////////////////    
//INCLUDES
//////////////////////////////////////////////////////////////
#include "SPI.h"
#include "General.h"
	
//////////////////////////////////////////////////////////////    
//DEFINES
//////////////////////////////////////////////////////////////
    //Comando de la EEPROM (Lectura, Escritura, Deshabilitar WR, Habilitar WR, Lectura STATUS Reg y Escritura STATUS Reg)
    #define EEPROM_CMD_READ     (BYTE)0b00000011
    #define EEPROM_CMD_WRITE    (BYTE)0b00000010
    #define EEPROM_CMD_WRDI     (BYTE)0b00000100
    #define EEPROM_CMD_WREN     (BYTE)0b00000110
    #define EEPROM_CMD_RDSR     (BYTE)0b00000101
    #define EEPROM_CMD_WRSR     (BYTE)0b00000001
    
    //Pines SCK, SDO, SDI y CS
    #define EEPROM_CS_TRIS      TRISDbits.TRISD12
    #define EEPROM_CS_PORT      PORTDbits.RD12
    #define EEPROM_SCK_TRIS     TRISGbits.TRISG6
    #define EEPROM_SDO_TRIS     TRISGbits.TRISG8
    #define EEPROM_SDI_TRIS     TRISGbits.TRISG7
 
//////////////////////////////////////////////////////////////    
//MACROS
//////////////////////////////////////////////////////////////

//Macro que extrae el byte bajo de una word
#define LowByte_Word(X)   (unsigned char)(X&0x00ff)
//Macro que extrae el byte alto de una word
#define HighByte_Word(X)   (unsigned char)((X>>8)&0x00ff)
//Macro que baja la línea de chip select 
#define mEEPROM_CS_Low()      EEPROM_CS_PORT=0;
//Macro que sube la línea de chip select
#define mEEPROM_CS_High()     EEPROM_CS_PORT=1;

//////////////////////////////////////////////////////////////    
//CONSTANTES
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////    
//TIPOS DEFINIDOS Y ESTRUCTURAS
//////////////////////////////////////////////////////////////

//Estructura para manejar el registro STATUS                                                                      *
struct  STATREG{
char    WIP:1;
char    WEL:1;
char    BP0:1;
char    BP1:1;
char    RESERVED:3;
char    WPEN:1;
};

//Union para tener acceso de bits y byte al registro STATUS
union _EEPROMStatus_{
struct  STATREG Bits;
char    Char;
};

//////////////////////////////////////////////////////////////    
//METODOS PUBLICOS   Definición de funciones o prototipos
//////////////////////////////////////////////////////////////
void EEPROM_Inicializacion();
union _EEPROMStatus_ EEPROM_ReadStatus();
void EEPROM_WriteByte(BYTE dato, WORD direccion);
BYTE EEPROM_ReadByte(WORD direccion);
void EEPROM_WriteEnable();


#endif
