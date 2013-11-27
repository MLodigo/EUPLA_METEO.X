#include <p24fj128ga010.h>
#include "General.h"
#include "Reloj_RTCC.h"
#include "leds.h"

/*********************************************************************
 * Function:        void Rtcc_Habilitar_Escritura(void)
 *
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        Permite la escritura de registros. RCFGCAL.RTCWREN.
 * Note:            Realiza la secuencia de desbloqueo.
 ********************************************************************/
void Rtcc_Habilitar_Escritura(void)
{
   UINT8  CPU_IPL;

/* Disable Global Interrupt */
   SET_AND_SAVE_CPU_IPL(CPU_IPL,7);

    asm volatile("disi #5"); 
    asm volatile("mov #0x55, w7"); 
    asm volatile("mov w7, _NVMKEY"); 
    asm volatile("mov #0xAA, w8"); 
    asm volatile("mov w8, _NVMKEY"); 
    asm volatile("bset _RCFGCAL, #13"); 
    asm volatile("nop"); 
    asm volatile("nop"); 
    
    RCFGCALbits.RTCWREN=1;  

/* Enable Global Interrupt */
   RESTORE_CPU_IPL(CPU_IPL);
}

/*********************************************************************
 * Function:        void Rtcc_Inicializacion(void)
 *
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        Inicializaci�n del m�dulo RTCC. Oscilador y calibraci�n est�ndar.
 *                  Deshabilitaci�n de escritura de registros y stop del reloj.
 ********************************************************************/
void Rtcc_Inicializacion(void)
{
   //Se habilita el oscilador secundario
   __builtin_write_OSCCONL(2);

   //Sin Calibraci�n aplicada
   RCFGCAL = 0x0000;
   
   //Desactivaci�n del reloj e inhabilitaci�n de la escritura
   if(mRtcc_Activo())
   {
      if(!mRtcc_Escritura_Habilitada())
      {
          Rtcc_Habilitar_Escritura();
      }
      mRtcc_Desactivar();
   }
   mRtcc_Deshabilitar_Escritura();
}

/*****************************************************************************************************
 * Function:        void Rtcc_Configuracion_FechaHora_Reloj(rtccFechaHora *pFechaHoraReloj)
 *
 * Overview:        Se inicializan, con los valores pasados como argumento, los registros del reloj.
 *                  Deshabilitaci�n de escritura de registros al finalizar.
 *****************************************************************************************************/
void Rtcc_Configuracion_FechaHora_Reloj(rtccFechaHora* pFechaHoraReloj)
{
    //Habilitamos la escrituta si no lo est�
    if(!mRtcc_Escritura_Habilitada())
    {
        Rtcc_Habilitar_Escritura();
    }
    
    //Paramos el reloj en el caso de estan activado
    mRtcc_Desactivar();
    
    //Situamos el puntero sobre correctamente sobre el primero de los registros RTCVAL
    //Con cada escritura sobre RTCVAL se decremente autom�ticamente.
    RCFGCALbits.RTCPTR = 3;								  	

    //Carga de valores pasados como argumento
    RTCVAL = pFechaHoraReloj->w[0];			//A�o
    RTCVAL = pFechaHoraReloj->w[1];			//Mes & Dia
    RTCVAL = pFechaHoraReloj->w[2];			//D�a Semana & Hora
    RTCVAL = pFechaHoraReloj->w[3];			//Minutos & Segundos
    
    //Bloqueamos nuevamente la escritura
    mRtcc_Deshabilitar_Escritura();
}

/*****************************************************************************************************
 * Function:        void Rtcc_Configuracion_FechaHora_Alarma(rtccFechaHora *pFechaHoraAlarma)
 *
 * Overview:        Se inicializan, con los valores pasados como argumento, los registros la alarma.
 *                  Deshabilitaci�n de escritura de registros al finalizar.
 *****************************************************************************************************/
void Rtcc_Configuracion_FechaHora_Alarma(rtccFechaHora *pFechaHoraAlarma)
{
    //Habilitamos la escrituta si no lo est�
    if(!mRtcc_Escritura_Habilitada())
    {
        Rtcc_Habilitar_Escritura();
    }
    
    //Paramos el reloj en el caso de estan activado
    mRtcc_Desactivar();
    
    //Situamos el puntero sobre correctamente sobre el primero de los registros RTCVAL
    //Con cada escritura sobre el registro ALRMVAL el puntero decrementa autom�ticamente.
    ALCFGRPTbits.ALRMPTR = 2;								  	
	
    //Carga de valores pasados como argumento
    ALRMVAL = pFechaHoraAlarma->w[1];			//Mes & Dia
    ALRMVAL = pFechaHoraAlarma->w[2];			//D�a Semana & Hora
    ALRMVAL = pFechaHoraAlarma->w[3];			//Minutos & Segundos
    
    //Bloqueamos nuevamente la escritura
    mRtcc_Deshabilitar_Escritura();
	
}

/**********************************************************************************************************************************************
 * Function:        void Rtcc_Configuracion_Opciones_Alarma(BOOL repeticionInfinita, rtccRepetirAlarma intervalo, BYTE iniContadorRepeticiones)
 *
 * Overview:        Configuraci�n del modo de funcionamiento de la alarma.
 *                  Deshabilitaci�n de escritura de registros al finalizar.
 ***********************************************************************************************************************************************/
void Rtcc_Configuracion_Opciones_Alarma(BOOL repeticionInfinita, rtccRepetirAlarma intervalo, BYTE iniContadorRepeticiones)
{
    //Habilitamos la escrituta si no lo est�
    if(!mRtcc_Escritura_Habilitada())
    {
        Rtcc_Habilitar_Escritura();
    }
    
    //Paramos el reloj en el caso de estar activado
    mRtcc_Desactivar();
    
    //Opciones configurables de la alarma
    ALCFGRPTbits.ALRMEN = 0;                          //Desactivaci�n de la alarma
    ALCFGRPTbits.CHIME = repeticionInfinita;          //Indica si cuando el contador ARPT se situe en 0x00, autom�ticamente se colocar� en 0xFF
    ALCFGRPTbits.AMASK = intervalo;                   //Indica el int�rvalo de tiempo transcurrido el cual saltar� la interrupcci�n de alarma
    ALCFGRPTbits.ARPT = iniContadorRepeticiones;      //Si CHIME=0, la alarma se repetir� el n�mero indicado aqu�. Una vez que sea 0x00, la alarma se desactivar�.
    ALCFGRPTbits.ALRMEN = 1;                          //Activaci�n de la alarma
	
    //Bloqueamos nuevamente la escritura
    mRtcc_Deshabilitar_Escritura();
}

/**********************************************************************************************************************************************
 * Function:        void Rtcc_Activacion(void)
 *
 * Overview:        Puesta en marcha del m�dulo RTCC.
 *                  Se habilita la interrupci�n asociada.
 ***********************************************************************************************************************************************/
void Rtcc_Activacion(void)
{
    //DesHabilitamos la interrupcci�n del RTCC
    IFS3bits.RTCIF = 0;
    IEC3bits.RTCIE = 0;
	
     //Habilitamos la escrituta si no lo est�
    if(!mRtcc_Escritura_Habilitada())
    {
        Rtcc_Habilitar_Escritura();
    }

    //Arranque del reloj
    mRtcc_Activar();
    
    //Bloqueamos nuevamente la escritura
    mRtcc_Deshabilitar_Escritura();
}

/**********************************************************************************************************************************************
 * Function:        void Lectura_FechaHora_Reloj(void)
 *
 * Overview:        Lectura del valor actual de los registros del reloj
 ***********************************************************************************************************************************************/
rtccFechaHora Lectura_FechaHora_Reloj(void)
{
    rtccFechaHora FechaHoraReloj;
    
    //Espera mientras no sea seguro realizar la lectura
    while(RCFGCALbits.RTCSYNC==1);                  
    
    //Se situa el puntero adecuadamente apuntando al registro 'Year'
    RCFGCALbits.RTCPTR = 3;                         
    
    FechaHoraReloj.w[0] = RTCVAL;
    FechaHoraReloj.w[1] = RTCVAL;
    FechaHoraReloj.w[2] = RTCVAL;
    FechaHoraReloj.w[3] = RTCVAL;

    return FechaHoraReloj;
}

/**********************************************************************************************************************************************
 * Function:        void Lectura_FechaHora_Alarma(void)
 *
 * Overview:        Lectura del valor a los que est� programada la alarma
 ***********************************************************************************************************************************************/
rtccFechaHora Lectura_FechaHora_Alarma(void)
{
    rtccFechaHora FechaHoraAlarma;
    
    //Espera mientras no sea seguro realizar la lectura
    while(RCFGCALbits.RTCSYNC==1);                  
    
    //Se situa el puntero adecuadamente apuntando al registro 'Mes-Dia'
    ALCFGRPTbits.ALRMPTR = 2;                         
    
    FechaHoraAlarma.w[1] = ALRMVAL;
    FechaHoraAlarma.w[2] = ALRMVAL;
    FechaHoraAlarma.w[3] = ALRMVAL;

    return FechaHoraAlarma;
}

/**********************************************************************************************************************************************
 * Function:        void _ISR _RTCCInterrupt(void)
 *
 * Overview:        Rutina de atenci�n a la interrupci�n del m�dulo RTCC.
 ***********************************************************************************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _RTCCInterrupt(void)
{
    rtccFechaHora FechaHoraReloj, FechaHoraAlarma;
    
    //Bajamos la bandera de interrupcci�n del RTCC
    IFS3bits.RTCIF = 0;
    
    //Lectura de la hora actual del reloj
    FechaHoraReloj = Lectura_FechaHora_Reloj();
    //Lectura de la hora de la alarma
    FechaHoraAlarma = Lectura_FechaHora_Alarma();
    
    if((FechaHoraReloj.f.DiaMes==FechaHoraAlarma.f.DiaMes) &&
       (FechaHoraReloj.f.Mes==FechaHoraAlarma.f.Mes) &&
       (FechaHoraReloj.f.DiaSemana==FechaHoraAlarma.f.DiaSemana) &&
       (FechaHoraReloj.f.Hora==FechaHoraAlarma.f.Hora) &&
       (FechaHoraReloj.f.Minutos==FechaHoraAlarma.f.Minutos) &&
       (FechaHoraReloj.f.Segundos==FechaHoraAlarma.f.Segundos))
    {
        LED9_ON;
        //Configuramos de las nuevas opciones de la alarma, repetici�n cada medio segundo, 10 veces..
        Rtcc_Configuracion_Opciones_Alarma(CHIME_OFF,RPT_MEDIO_SEG, 0x0A);
        //Activaci�n del reloj
        Rtcc_Activacion();
        LED9_OFF;
    } 
    else
    {
        if(IS_LED8_ON){LED8_OFF;}
        else{LED8_ON;}
    }   
}
