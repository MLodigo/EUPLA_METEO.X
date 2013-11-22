#include "SPI.h"


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                VARIABLES MIEMBRO
//********************************************************************************************************************    
//********************************************************************************************************************    


/************************************************************************
* Precondiciones: 
*	Bit TRIS SCK, SDO y CS como output
*	Bit TRIS SDI como input
*																		
* Descripción:															
	Inicializa el módulo con las opciones de maestro                    
************************************************************************/ 
void SPI_Inicializacion()
{
	//Módulo SPI deshabilitado
	SPISTAT = 0;
 
	//Modo Maestro
	//Prescalers primario y secundario a 1:1. Fsys = 8MHz -> Fcy = 4MHz y la memoria trabaja a 4MHz (5MHz típico)
	//Polaridad reloj: Estado Activo a nivel bajo, Estado inactivo a nivel alto
	//Flanco activo reloj: Flanco de subida seleccionado, desde estado inactivo a activo.
	//SSEN desactivado. No se produce pulso de sincronismo.
	//Muestreo de dato de entrada al final del tiempo del dato de salida
    //Datos de 8 bits
    //Pin SDO controlado por el módulo
    //Relos interno para pin SDI activado
	SPICON1 = 0x027F;; 
    
    //Modo Framed deshabilitado
    SPICON2 = 0;
    
	//Interrupción del módulo deshabilitada
    SPIINTENbits.SPIIE = 0;
    SPIINTFLGbits.SPIIF = 0;
    
	//Módulo SPI habilitado
	SPISTATbits.SPIEN = 1;
}

/************************************************************************
* Precondiciones: 
*	'SPI_Inicializacion' debería haber sido llamada.
*																		
* Descripción:															
*	Espera a que se permita transmitir, y entonces envia un dato
************************************************************************/ 
BOOL SPI_EscribeDato(BYTE dato)
{
	//Espera a que la bandera de "Transmitiendo" se baje..
    while(SPISTATbits.SPITBF);
    SPIBUF = dato;
    return TRUE;
}

/************************************************************************
* Precondiciones: 
*	‘SPI_EscribeDato’ debería haber sido llamada.
*																		
* Descripción:															
*	Espera que la transmisión del byte se complete	
************************************************************************/ 
BOOL SPI_TransmisionFinalizada()
{
	//Por construcción, en el bus SPI, al enviar un dato, recibes tambien otro.
	//Por tanto, para verificar la correcta transmisión, espera a recibir dato. 
	while(SPISTATbits.SPIRBF == 0);
    return TRUE;
}


