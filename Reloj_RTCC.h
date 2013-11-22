#ifndef __RTCCPIC24_H
 #define __RTCCPIC24_H
 
#include "General.h"

///////////////////////////////////////////////////////////////   DEFINICION DE TIPOS Y ENUMERACIONES   ////////////////////////////////////////
//Union-Estructura para lectura/escritura de FECHA y HORA en el RTCC
typedef union
{ 
    struct
    {
        BYTE    Ano;        // Codificación BCD para año,              00-99
        BYTE    Rsvd;       // Reservado
        BYTE    DiaMes;     // Codificación BCD para día del mes,      01-31
        BYTE    Mes;        // Codificación BCD para mes,              01-12
        BYTE    Hora;       // Codificación BCD para horas,            00-24
        BYTE    DiaSemana;  // Codificación BCD para día de la semana, 00-06   
        BYTE    Segundos;   // Codificación BCD para segundos,         00-59 
        BYTE    Minutos;    // Codificación BCD para minutos,          00-59
    }f;                     // Acceso a cada campo
    BYTE        b[8];       // Acceso BYTE   8 bits
    UINT16      w[4];       // Acceso WORD  16 bits 
    UINT32      l[2];       // Acceso DWORD 32 bits
}rtccFechaHora;

//Valores válidos de repetición de la alarma AMASK<3:0>
typedef enum
{
    RPT_MEDIO_SEG,     // Repetir alarma cada medio segundo
    RPT_SEGUNDO,       // Repetir alarma cada segundo
    RPT_DIEZ_SEG,      // Repetir alarma cada 10 segundos
    RPT_MINUTO,        // Repetir alarma cada minuto
    RPT_DIEZ_MIN,      // Repetir alarma cada 10 minutos
    RPT_HORA,          // Repetir alarma cada hora
    RPT_DIA,           // Repetir alarma cada día
    RPT_SEMANA,        // Repetir alarma cada semana 
    RPT_MES,           // Repetir alarma cada mes
    RPT_ANO            // Repetir alarma cada año (excepto cuando se configura con año bisiesto) 
}rtccRepetirAlarma;

//////////////////////////////////////////////////////////////////////   DEFINES    ///////////////////////////////////////////////////////////////// 

typedef enum
{
    CHIME_ON = 1,
    CHIME_OFF = 0
}CHIME;
/*********************************************************************
 * Macro:           mRtcc_Activo()
 * Input:           None
 * Output:          Devuelve el valor de RCFGCAL.RTCEN
 * Note:            ¿Está habilitado el módulo RTCC?
 ********************************************************************/
#define mRtcc_Activo()   (RCFGCALbits.RTCEN)
/*********************************************************************
 * Macro:           mRtcc_Escritura_Habilitada()
 * Input:           None
 * Output:          Indica si está la escritura habilitada sobre los 
 *                  registros de control.
 ********************************************************************/
#define mRtcc_Escritura_Habilitada()   (RCFGCALbits.RTCWREN)
/*********************************************************************
 * Macro:           mRtcc_Activar()
 * Input:           None
 * Output:          Arranca el reloj RTCC
 ********************************************************************/
#define mRtcc_Activar()   (RCFGCALbits.RTCEN = 1)
/*********************************************************************
 * Macro:           mRtcc_Desactivar()
 * Input:           None
 * Output:          Apaga el reloj RTCC
 ********************************************************************/
#define mRtcc_Desactivar()   (RCFGCALbits.RTCEN = 0)
/*********************************************************************
 * Macro:           mRtcc_Deshabilitar_Escritura()
 * Input:           None
 * Output:          Activa protección de escritura.
 ********************************************************************/
#define mRtcc_Deshabilitar_Escritura()   (RCFGCALbits.RTCWREN = 0)


//////////////////////////////////////////////////////////////////////   PROTOTIPOS DE FUNCIONES    ////////////////////////////////////////////////////// 
extern void Rtcc_Inicializacion(void) __attribute__ ((section (".libperi")));
extern void Rtcc_Configuracion_FechaHora_Reloj(rtccFechaHora *pFechaHoraReloj) __attribute__ ((section (".libperi")));
extern void Rtcc_Configuracion_FechaHora_Alarma(rtccFechaHora *pFechaHoraAlarma) __attribute__ ((section (".libperi")));
extern void Rtcc_Configuracion_Opciones_Alarma(BOOL repeticionInfinita, rtccRepetirAlarma intervalo, BYTE iniContadorRepeticiones) __attribute__ ((section (".libperi")));
extern void Rtcc_Activacion(void) __attribute__ ((section (".libperi")));
extern rtccFechaHora Lectura_FechaHora_Reloj(void) __attribute__ ((section (".libperi")));

#endif









