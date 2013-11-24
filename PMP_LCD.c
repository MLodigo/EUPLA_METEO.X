#include "PMP_LCD.h"
#include "p24fxxxx.h"
#include "General.h"

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                VARIABLES MIEMBRO
//********************************************************************************************************************    
//********************************************************************************************************************    


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                               METODOS PRIVADOS
//********************************************************************************************************************    
//********************************************************************************************************************    

//Prototipos
PRIVATE void Wait(unsigned int B);
PRIVATE void LCD_Write(unsigned char tipo_Instruc_Dato, unsigned char dato);
PRIVATE char LCD_Read(int direccion);
PRIVATE void LCD_SituaCursor(int numLinea, int posicion);

/*********************************************************************
 * Precondici�n: Ninguna
 *
 * Descripci�n: Funci�n que realiza un retardo.
 *              Para un frecuencia de 8 MHz, el argumento indicar� microsegundos.
 *              La funcion de inicializaci�n del LCD est� pensada con retardos a esta frecuencia de reloj.              
 ********************************************************************/
PRIVATE void Wait(unsigned int B)
{
    while(B)
    {
       B--;
    }
}

/*********************************************************************
 * Precondici�n: Debe haberse llamado anteriormente la funci�n LCD_Inicializacion()
 *
 * Descripci�n: Realiza una escritura en el m�dulo LCD, ya sea una instrucci�n o un dato
 ********************************************************************/
PRIVATE void LCD_Write(unsigned char tipo_Instruc_Dato, unsigned char dato)
{
    while( LCD_Ocupado());
    while( PMMODEbits.BUSY);
    PMADDR = tipo_Instruc_Dato;
    PMDIN1 = dato;
}


/*********************************************************************
 * Precondici�n: Debe haberse llamado anteriormente la funci�n LCD_Inicializacion()
 *
 * Descripci�n: Lectura del registro de estado del LCD
 ********************************************************************/
PRIVATE char LCD_Read(int direccion)
{
    int dummy;
    while( PMMODEbits.BUSY);    //Espera a liberarse el m�dulo PMP
    PMADDR = direccion;         //Selecciona la direccion comando
    dummy = PMDIN1;             //Inicio un ciclo de lectura leyendo un dummy byte
    while( PMMODEbits.BUSY);    //Espera a liberarse el m�dulo PMP
    return( PMDIN1);            //Lectura del registro de estado
    
}

/*********************************************************************
 * Precondici�n: Debe haberse llamado anteriormente la funci�n LCD_Inicializacion()
 *
 * Descripci�n: Situa el cursor en la posci�n y l�nea indicada
 ********************************************************************/
PRIVATE void LCD_SituaCursor(int numLinea, int posicion)
{
    if(numLinea == LCD_LINEA1)
    {
            LCD_Write(LCD_I,(posicion & 0x0f)|0x80);
    }
    else if(numLinea == LCD_LINEA2)
    {
            LCD_Write(LCD_I,(posicion & 0x0f)|0xc0);
    }
}

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                METODOS PUBLICOS
//********************************************************************************************************************    
//********************************************************************************************************************  


/*********************************************************************
 * Precondici�n: Debe haberse llamado anteriormente la funci�n LCD_Inicializacion()
 *
 * Descripci�n: Limpia la pantalla y cursor en posici�n 0.
 ********************************************************************/
void LCD_Clear()
{
    LCD_Write(LCD_I, LCD_CLEAR_DISPLAY);
}
 

/*********************************************************************
 * Precondici�n: Debe haberse llamado anteriormente la funci�n LCD_Inicializacion()
 *
 * Descripci�n: Escribe una cadena de 16 car�cteres de tama�o obligatoriamente en la l�nea
 *              del display indicada desde la posici�n 0.
 *              �til cuando se quiere escribir la l�nea completa desde la posci�n 0 a 15
 ********************************************************************/
void LCD_WriteLinea(unsigned char numLinea, unsigned char * cadenaDisplay)
{
    unsigned char CntCaracter;

    unsigned char CeroEncontrado = 0;


    //Situamos el cursor al principio de la l�nea indicada
    if (numLinea==LCD_LINEA1)
            LCD_Write(LCD_I, LCD_DDRAM1(0));	//Cursor en el primer car�cteres de la l�nea 1
    else if (numLinea==LCD_LINEA2)
            LCD_Write(LCD_I, LCD_DDRAM2(0));	//Cursor en el primer car�cteres de la l�nea 2

    //Escritura de todos los car�cteres de la cadena
    for (CntCaracter = 0; CntCaracter < LCD_DISPLAY_LEN; CntCaracter++)
    {
        if((*cadenaDisplay != 0)&&(!CeroEncontrado))
        {
            LCD_Write(LCD_D, *cadenaDisplay);

        }else
        {
            CeroEncontrado = 1;
            LCD_Write(LCD_D, ' ');
        }
        cadenaDisplay++;
    }
}

/*********************************************************************
 * Precondici�n: Debe haberse llamado anteriormente la funci�n LCD_Inicializacion()
 *
 * Descripci�n: Escribe el texto indicado, en la l�nea y posici�n indicada
 ********************************************************************/
void LCD_WrString_LinPos(int numLinea, int posicion, unsigned char *cadena)
{
    LCD_SituaCursor(numLinea, posicion);
    while( *cadena) LCD_Write(LCD_D, *cadena++);
} 

/*********************************************************************
 * Precondici�n: Ninguna
 *
 * Descripci�n: Configura en m�dulo PMP y el m�dulo LCD
 ********************************************************************/
void LCD_Inicializacion()
{
    //REGISTRO PMCON//////////////////////////////////////////////////
    //PMP habilitado
    PMCONbits.PMPEN		= 1;
    //Sigue funcionando en Idle mode
    PMCONbits.PSIDL		= 0;
    //No se multiplexan datos y direcciones en el mismo bus
    PMCONbits.ADRMUX	= 0;
    //Byte habilitado Puerto deshabilitado
    PMCONbits.PTBEEN	= 0;
    //Escritura habilitada Puerto Strobe habilitado
    PMCONbits.PTWREN	= 1;
    //Lectura/Escritura Puerto Strobe habilitada
    PMCONbits.PTRDEN	= 1;
    //PMCS1 and PMCS2 funcionan como chip select
    PMCONbits.CSF		= 2;
    //Polaridad Address Latch Activa High (PMALL and PMALH)
    PMCONbits.ALP		= 1;
    //Chip Select 2 Polaridad Activa High
    PMCONbits.CS2P		= 1;
    //Chip Select 1 Polaridad Activa High
    PMCONbits.CS1P		= 1;
    //Byte Enable Activo High
    PMCONbits.BEP		= 1;
    //Modo Master, Write Strobe Activo High
    PMCONbits.WRSP		= 1;
    //Modo Master, Read/Write Strobe Activo High
    PMCONbits.RDSP		= 1;

    //REGISTRO PMMODE////////////////////////////////////////////////////
    //No genera interrupcci�n
    PMMODEbits.IRQM		= 0;
    //No incrementa o decrementa direccion
    PMMODEbits.INCM		= 0;
    //Modo de datos de 8-bit
    PMMODEbits.MODE16	= 0;
    //Modo Master 1(PMCSx, PMRD/PMWR, PMENB, PMBE, PMA<x:0> and PMD<7:0>)
    PMMODEbits.MODE		= 3;
    //Dato espera 4Tcy; multiplexed address phase of 4 Tcy
    PMMODEbits.WAITB	= 3;
    //Carga habilitada de lectura de dato: Espera 15 Tcy adicionales
    PMMODEbits.WAITM	= 0xf;
    //Mantenimiento despues de la carga del dato: Espera 4 Tcy
    PMMODEbits.WAITE	= 3;

    //REGISTRO PMADDR////////////////////////////////////////////////////
    PMADDR 	= 0x0000;

    //REGISTRO PMAEN/////////////////////////////////////////////////////
    // PMA15:2 Como l�neas de puerto I/O, PMALH/PMALL enabled
    PMAEN 	= 0x0001;

    //Retardo de 40 mseg. por configuraci�n del PMP
    Wait(40000);

    //Function Set: 8 bits de datos
    LCD_Write(LCD_I, LCD_DATOS_8_BITS);


    //Display ON, Cursor OFF y Parpadeo Cursor OFF
    LCD_Write(LCD_I, LCD_DIS_ON_CUR_OFF_PAR_OFF);


    //Entry Mode: Movimiento del cursor Incremental
    LCD_Write(LCD_I, LCD_CURSOR_INC_NS);


    //Se desplaza el cursor, hacia la derecha
    LCD_Write(LCD_I, LCD_CURSOR_NO_SHIFT);

    //Limpia display: Borra RAM, Cursor posicion 0.
    LCD_Clear();
}



