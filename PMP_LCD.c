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
 * Precondición: Ninguna
 *
 * Descripción: Función que realiza un retardo.
 *              Para un frecuencia de 8 MHz, el argumento indicará microsegundos.
 *              La funcion de inicialización del LCD está pensada con retardos a esta frecuencia de reloj.              
 ********************************************************************/
PRIVATE void Wait(unsigned int B)
{
    while(B)
    {
       B--;
    }
}

/*********************************************************************
 * Precondición: Debe haberse llamado anteriormente la función LCD_Inicializacion()
 *
 * Descripción: Realiza una escritura en el módulo LCD, ya sea una instrucción o un dato
 ********************************************************************/
PRIVATE void LCD_Write(unsigned char tipo_Instruc_Dato, unsigned char dato)
{
    while( LCD_Ocupado());
    while( PMMODEbits.BUSY);
    PMADDR = tipo_Instruc_Dato;
    PMDIN1 = dato;
}


/*********************************************************************
 * Precondición: Debe haberse llamado anteriormente la función LCD_Inicializacion()
 *
 * Descripción: Lectura del registro de estado del LCD
 ********************************************************************/
PRIVATE char LCD_Read(int direccion)
{
    int dummy;
    while( PMMODEbits.BUSY);    //Espera a liberarse el módulo PMP
    PMADDR = direccion;         //Selecciona la direccion comando
    dummy = PMDIN1;             //Inicio un ciclo de lectura leyendo un dummy byte
    while( PMMODEbits.BUSY);    //Espera a liberarse el módulo PMP
    return( PMDIN1);            //Lectura del registro de estado
    
}

/*********************************************************************
 * Precondición: Debe haberse llamado anteriormente la función LCD_Inicializacion()
 *
 * Descripción: Situa el cursor en la posción y línea indicada
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
 * Precondición: Debe haberse llamado anteriormente la función LCD_Inicializacion()
 *
 * Descripción: Limpia la pantalla y cursor en posición 0.
 ********************************************************************/
void LCD_Clear()
{
    LCD_Write(LCD_I, LCD_CLEAR_DISPLAY);
}
 

/*********************************************************************
 * Precondición: Debe haberse llamado anteriormente la función LCD_Inicializacion()
 *
 * Descripción: Escribe una cadena de 16 carácteres de tamaño obligatoriamente en la línea
 *              del display indicada desde la posición 0.
 *              Útil cuando se quiere escribir la línea completa desde la posción 0 a 15
 ********************************************************************/
void LCD_WriteLinea(unsigned char numLinea, unsigned char * cadenaDisplay)
{
    unsigned char CntCaracter;

    unsigned char CeroEncontrado = 0;


    //Situamos el cursor al principio de la línea indicada
    if (numLinea==LCD_LINEA1)
            LCD_Write(LCD_I, LCD_DDRAM1(0));	//Cursor en el primer carácteres de la línea 1
    else if (numLinea==LCD_LINEA2)
            LCD_Write(LCD_I, LCD_DDRAM2(0));	//Cursor en el primer carácteres de la línea 2

    //Escritura de todos los carácteres de la cadena
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
 * Precondición: Debe haberse llamado anteriormente la función LCD_Inicializacion()
 *
 * Descripción: Escribe el texto indicado, en la línea y posición indicada
 ********************************************************************/
void LCD_WrString_LinPos(int numLinea, int posicion, unsigned char *cadena)
{
    LCD_SituaCursor(numLinea, posicion);
    while( *cadena) LCD_Write(LCD_D, *cadena++);
} 

/*********************************************************************
 * Precondición: Ninguna
 *
 * Descripción: Configura en módulo PMP y el módulo LCD
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
    //No genera interrupcción
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
    // PMA15:2 Como líneas de puerto I/O, PMALH/PMALL enabled
    PMAEN 	= 0x0001;

    //Retardo de 40 mseg. por configuración del PMP
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



