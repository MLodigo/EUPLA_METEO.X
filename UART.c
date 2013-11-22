
/*

  Inclusión de librerías
  ...
  Definiciones de cuerpos de funciones (cuyos prototipos ya fueron declarados)
  ...
  Interrupciones
  ...

*/
#include "General.h"
#include "UART.h"
#include <p24FJ128GA010.h> 


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                VARIABLES MIEMBRO
//********************************************************************************************************************    
//********************************************************************************************************************    

/*
//UART1
#define SIZE_BUFFER_UART1 250  //No superar los 255 (unsigned char)
static unsigned char UART1_RxBuffer[SIZE_BUFFER_UART1];
static unsigned char UART1_Puntero_Rx_IN = 0;
static unsigned char UART1_Puntero_Rx_OUT = 0;
static unsigned char UART1_Cnt_Datos_Por_Leer = 0;
static unsigned char UART1_RebaseBuffer = FALSE;
static unsigned char UART1_SobrescrituraBuffer= FALSE;
*/
//UART2
#define SIZE_BUFFER_UART2 250  //No superar los 255 (unsigned char)
PRIVATE unsigned char UART2_RxBuffer[SIZE_BUFFER_UART2];
PRIVATE unsigned char UART2_Puntero_Rx_IN = 0;
PRIVATE unsigned char UART2_Puntero_Rx_OUT = 0;
PRIVATE unsigned char UART2_Cnt_Datos_Por_Leer = 0;
PRIVATE unsigned char UART2_RebaseBuffer = FALSE;
PRIVATE unsigned char UART2_SobrescrituraBuffer= FALSE;

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                               METODOS PRIVADOS
//********************************************************************************************************************    
//********************************************************************************************************************    

//Prototipos
PRIVATE unsigned char UART2_Lee_Dato_Buffer_Recepcion(void);
PRIVATE void UART2_Reset_Variables_Control_Buffer(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Lee dato recibido en el buffer del la UART2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRIVATE unsigned char UART2_Lee_Dato_Buffer_Recepcion(void)
{
	return( U2RXREG );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Inicializa la variables de control del buffer de recepción de la UART2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRIVATE void UART2_Reset_Variables_Control_Buffer(void)
{
    UART2_Puntero_Rx_IN = 0;
    UART2_Puntero_Rx_OUT = 0;
    UART2_Cnt_Datos_Por_Leer = 0;
    UART2_RebaseBuffer = FALSE;
    UART2_SobrescrituraBuffer= FALSE;
}    


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                 METODOS PUBLICOS
//********************************************************************************************************************
//********************************************************************************************************************    

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Inicialización UART2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UART2_Configura_Abre(void)
{
    UART2_Reset_Variables_Control_Buffer();
    
	U2MODEbits.UARTEN = 0;	//Deshabilita UART
	U2MODEbits.USIDL = 0;   //Continua operación en IDLE Mode
	U2MODEbits.IREN = 0;    //IrDA deshabilitado
	U2MODEbits.RTSMD = 1;   //Modo para RTS (Request to Send)
	U2MODEbits.UEN = 0;     //TX y RX habilitados, CTS y RTS deshabilitados.
	U2MODEbits.WAKE = 0;    //No de despierta con un bit de Start en modo Sleep
	U2MODEbits.LPBACK = 0;  //Modo Loopback deshabilitado. No se conecta la transmisión con la recepción.
	U2MODEbits.ABAUD = 0;   //AutoBaudRate desactivado.
	U2MODEbits.RXINV = 0;   //UxRX en modo Idle es 1
	U2MODEbits.BRGH = 0;    //Low speed
	U2MODEbits.PDSEL = 0;   //Sin paridad y 8 bits de datos
	U2MODEbits.STSEL = 0;   //1 bit de Stop
	
	//Carga del valor en el generador de BaudRate. 9600 bps.
	//Ver sección 21.3. del datasheet.
	//U2BRG = (Fcy/(16*BaudRate))-1
	//U2BRG = (4M/(16*9600))-1
	U2BRG = 25;	// 8Mhz osc, 9600 Baud
    
    U2STAbits.UTXISEL1 = 0;  //Interrupcción de transmisión cuando el dato es transferido al registro Shift
	U2STAbits.UTXISEL0 = 0;
	U2STAbits.UTXINV = 0;    //UxTX en modo Idle es 0
	U2STAbits.UTXBRK = 0;    //Transmisión con Sync Break deshabilitada
	U2STAbits.UTXEN = 0;     //Transmisión deshabilitada
	U2STAbits.UTXBF = 0;     //Bandera que indica que el buffer de transmisión está lleno. Solo de lectura. 
	U2STAbits.TRMT = 0;      //Bandera que indica que el registro Shift de transmisión está vacio. Solo de lectura.
	U2STAbits.URXISEL = 0;   //Interrupción de recepción cuando se recibe un dato
	U2STAbits.ADDEN = 0;     //Detección de dirección deshabilitado. Modo Multiprocesador.
	U2STAbits.RIDLE = 0;     //Solo lectura.
	U2STAbits.PERR = 0;      //Bandera de error de paridad. Solo lectura.
    U2STAbits.FERR = 0;      //Bandera de error de trama. Solo lectura.
    U2STAbits.OERR = 0;      //Bandera de error de over-run del buffer de recepción. Solo lectura.
    U2STAbits.URXDA = 0;     //Bandera que indica si existe un dato en el buffer por leer. Solo lectura.
	
	//IPC7 = 0x4400;	// Mid Range Interrupt Priority level, no urgent reason

	IFS1bits.U2TXIF = 0;	 //Limpia la bandera de transmisión realizada
	IEC1bits.U2TXIE = 1;	 //Habilita la interrupción de transmisión
	IFS1bits.U2RXIF = 0;	 //Limpia la bandera de recepción realizada
	IEC1bits.U2RXIE = 1;	 //Habilita la interrupción de recepción

	U2MODEbits.UARTEN = 1;	 //Activación de la UART2
	U2STAbits.UTXEN = 1;     //Obligatorio: Habilitar transmisión justo despues del periférico. 
	
	while( U2STAbits.URXDA ){ UART2_Lee_Dato_Buffer_Recepcion(); } //Vacia el buffer de rxd
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Deshabilita UART2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UART2_Cierra(void)
{  
    U2MODEbits.UARTEN = 0;	
    IEC1bits.U2RXIE = 0;
    IEC1bits.U2TXIE = 0;
    IFS1bits.U2RXIF = 0;
    IFS1bits.U2TXIF = 0;
}

void UART2_Configura_Modo_Sleep(void)
{
    U2MODEbits.WAKE = 1;
}

//Devuelve si se produjo sobreescritura en el buffer de recepción UART (Datos perdidos). 
//Para que externamente se tomen acciones como la de repetir la comunicación.
BOOL UART2_Hay_Sobrescritura_Buffer_RX(void)
{
    return UART2_SobrescrituraBuffer;
}

//Determina si hay datos en el buffer UART2 por leer
UINT8 UART2_DatosPorLeer(void)
{
    return UART2_Cnt_Datos_Por_Leer;
    
}

//Realiza una lectura de los datos indicados sobre el buffer UART2
//Hay que comprobar primero que hay datos por leer.
UINT8 UART2_LeerDatos(char *buffer, UINT8 numDatos)
{
    UINT8 CntDatosLeidos = 0;

    //Si se piden más datos de los que hay almacenados, ajustamos
    if (numDatos > UART2_Cnt_Datos_Por_Leer)
    {
        numDatos = UART2_Cnt_Datos_Por_Leer;
    }
    
    //Lectura solicitada
    while(CntDatosLeidos < numDatos)
    {
        *buffer = UART2_RxBuffer[UART2_Puntero_Rx_OUT]; //Devolvemos los datos almacenados con el apuntador OUT
        buffer++;                                       //Incrementamos el puntero al buffer de salida
        UART2_Puntero_Rx_OUT++;                         //Incrementamos el apuntador sobre el buffer de la UART
        CntDatosLeidos++;                               //Incrementamos el contador de número de datos a devolver
        UART2_Cnt_Datos_Por_Leer--;                     //Decrementamos variable miembro que indica los datos por leer en buffer UART
        //Si se produce rebase del buffer..
        if(UART2_Puntero_Rx_OUT==SIZE_BUFFER_UART2)    
        {
            UART2_Puntero_Rx_OUT = 0;
            UART2_RebaseBuffer = FALSE;
        }    
    }   
    //Finalmente devolvemos la cantidad de datos leidos
    return CntDatosLeidos;
}

//Lanza un byte por la UART2
void UART2_Envia_Byte(unsigned char dato)
{
    //Espera a que el buffer de transmisión esté vació
    while(U2STAbits.UTXBF == 1);
    //Espera a que la última transmisión se complete
    while(U2STAbits.TRMT == 0);
    //Envio del dato
    U2TXREG = dato;
    //Espera a que la última transmisión se complete
    while(U2STAbits.TRMT == 0);
}

//Lanza un array de bytes por la UART2
void UART2_Envia_Cadena(char *cadena)
{
    //Hasta llegar al final de la cadena..
    while(*cadena != '\0')
    {
        UART2_Envia_Byte(*cadena);
        cadena++;
    }
}

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                INTERRUPCIONES
//********************************************************************************************************************    
//********************************************************************************************************************    

//ISR Recepción UART1
void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void) 
{
	IFS0bits.U1RXIF = 0;
}

//ISR Transmisión UART1
void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void) 
{
	IFS0bits.U1TXIF = 0;
}

//ISR Recepción UART2
void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void) 
{
    //Comprobación de que al menos hay un dato por leer
    if(U2STAbits.URXDA)
    {
    	do
    	{
            //Comprobación de que no hay errores de frame y paridad
        	if((!U2STAbits.FERR)&&(!U2STAbits.PERR))
        	{
                UART2_RxBuffer[UART2_Puntero_Rx_IN] = UART2_Lee_Dato_Buffer_Recepcion();
                UART2_Cnt_Datos_Por_Leer++;
                UART2_Puntero_Rx_IN++;
                //Si se produce rebase del buffer..
                if(UART2_Puntero_Rx_IN==SIZE_BUFFER_UART2)
                {
                    UART2_Puntero_Rx_IN = 0;
                    UART2_RebaseBuffer = TRUE;
                }
                //Si hubo rebase y los apuntadores coinciden..Sobreescritura.
                if((UART2_RebaseBuffer)&&(UART2_Puntero_Rx_IN == UART2_Puntero_Rx_OUT))
                {
                   UART2_SobrescrituraBuffer = TRUE;
                }
        	}
        	else
        	{
                UART2_Lee_Dato_Buffer_Recepcion();	
        	}
        }
        while(U2STAbits.URXDA); //Bucle de lectura de datos mientras existan en el buffer de recepción   	
    }
    else
    {
        UART2_Lee_Dato_Buffer_Recepcion();	
    }
    
    //Flag de interrupcción deshabilitado
    IFS1bits.U2RXIF = 0;
}

//ISR Transmisión UART2
void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void) 
{
	IFS1bits.U2TXIF = 0;
}













