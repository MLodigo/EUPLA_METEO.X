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
* Descripci�n:															
	Inicializa el m�dulo con las opciones de maestro                    
************************************************************************/ 
void SPI_Inicializacion()
{
	//M�dulo SPI deshabilitado
	SPISTAT = 0;
 
	//Modo Maestro
	//Prescalers primario y secundario a 1:1. Fsys = 8MHz -> Fcy = 4MHz y la memoria trabaja a 4MHz (5MHz t�pico)
	//Polaridad reloj: Estado Activo a nivel bajo, Estado inactivo a nivel alto
	//Flanco activo reloj: Flanco de subida seleccionado, desde estado inactivo a activo.
	//SSEN desactivado. No se produce pulso de sincronismo.
	//Muestreo de dato de entrada al final del tiempo del dato de salida
    //Datos de 8 bits
    //Pin SDO controlado por el m�dulo
    //Relos interno para pin SDI activado
	SPICON1 = 0x027F;; 
    
    //Modo Framed deshabilitado
    SPICON2 = 0;
    
	//Interrupci�n del m�dulo deshabilitada
    SPIINTENbits.SPIIE = 0;
    SPIINTFLGbits.SPIIF = 0;
    
	//M�dulo SPI habilitado
	SPISTATbits.SPIEN = 1;
}

/************************************************************************
* Precondiciones: 
*	'SPI_Inicializacion' deber�a haber sido llamada.
*																		
* Descripci�n:															
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
*	�SPI_EscribeDato� deber�a haber sido llamada.
*																		
* Descripci�n:															
*	Espera que la transmisi�n del byte se complete	
************************************************************************/ 
BOOL SPI_TransmisionFinalizada()
{
	//Por construcci�n, en el bus SPI, al enviar un dato, recibes tambien otro.
	//Por tanto, para verificar la correcta transmisi�n, espera a recibir dato. 
	while(SPISTATbits.SPIRBF == 0);
    return TRUE;
}


