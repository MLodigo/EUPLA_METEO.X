/*  EUPLA METEO ===============================================================
 *  Programa para placa de desarrollo Explorer 16 de Microchip basada en PIC24F
 *  Proyecto final de carrera en Ingenier�a T�cnica Inform�tica de Sistemas
 */

#include <p24fj128ga010.h>
#include "App_Main.h"

//********************************************************************************************************************
//Palabras de configuracion del microprocesador PIC24 de Microchip.
//********************************************************************************************************************
/*CONFIGURATION BITS: FLASH CONFIGURATION WORD 1
	JTAGEN_OFF -> Puerto JTAG deshabilitado
	GCP_OFF	   -> Protecci�n de c�digo deshabilitado
	GWRP_OFF   -> Escrituras en memoria de programa deshabilitado
	COE_OFF    -> Modo emulaci�n clip-on deshabilitado (Fijado siempre as�)
	FWDTEN_OFF -> Watchdog Timer habilitado. Configurado en 8 segundos.
	ICS_PGx2   -> Pines del Emulador en EMUC2 y EMUD2
*/
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & FWPSA_PR128 & WDTPS_PS1024 & ICS_PGx2)
/*CONFIGURATION BITS: FLASH CONFIGURATION WORD 2
	FNOSC_PRI    -> Oscilador Primario sin PLL (8MHz) 
	POSCMOD_XT   -> Oscilador en modo XT (Consumo y prestacionse medias)
	FCKSM_CSDCMD -> Clock Switching deshabilitado y Monitorizaci�n del reloj deshabilitada.
        OSCIOFNC_OFF -> Configuraci�n del pin OSC2
*/
_CONFIG2( FNOSC_PRI & POSCMOD_XT & FCKSM_CSDCMD & OSCIOFNC_OFF )

//********************************************************************************************************************
//Programa EUPLA METEO. Punto de entrada de ejecuci�n del programa.
//********************************************************************************************************************
int main(void)
{
    //Inicializaci�n de m�dulos tras un Reset
    Inicializa_Sistema();

    //Bucle principal
    while(1)
    {
        Procesa_Evento_Despierte();
        Modo_Sleep();
    } 
}