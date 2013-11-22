#include "ADC.h"
#include <p24fj128ga010.h>
#include "General.h"

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                VARIABLES MIEMBRO
//********************************************************************************************************************    
//********************************************************************************************************************    
PRIVATE SENSORES Sensores; //Guarda la �ltima medida de los valores de los sensores y bateria
PRIVATE BOOL NuevaMedidaTomada = FALSE; //Indica si hay nuevas medidas que no se hayan leido todav�a

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                M�TODOS PRIVADOS
//********************************************************************************************************************    
//********************************************************************************************************************    
void Actualiza_Sensores(void);

//Funci�n que mantiene actualizada la variables Sensores y Bater�a tras cada medida realizada por el conversor ADC
void Actualiza_Sensores(void) {
    //Media de Temperatura
    Sensores.Temperatura = ADC1BUF2 + ADC1BUF6 + ADC1BUFA + ADC1BUFE;
    Sensores.Temperatura = (DWORD) (Sensores.Temperatura / 4);
    Sensores.Temperatura = (DWORD) (((Sensores.Temperatura * 3300) / 1024) - 525);

    //Media de Pluviometria
    Sensores.Pluviometria = ADC1BUF3 + ADC1BUF7 + ADC1BUFB + ADC1BUFF;
    Sensores.Pluviometria = (DWORD) (Sensores.Pluviometria / 4);
    Sensores.Pluviometria = (DWORD) ((Sensores.Pluviometria * 3300) / 1024);

    //Medida de Velocidad Aire
    Sensores.Vel_Aire = ADC1BUF1 + ADC1BUF5 + ADC1BUF9 + ADC1BUFD;
    Sensores.Vel_Aire = (DWORD) (Sensores.Vel_Aire / 4);
    Sensores.Vel_Aire = (DWORD) ((Sensores.Vel_Aire * 3300) / 1024);

    //Medida de Estado Bater�a
    Sensores.Nivel_Bateria = ADC1BUF0 + ADC1BUF4 + ADC1BUF8 + ADC1BUFC;
    Sensores.Nivel_Bateria = (DWORD) (Sensores.Nivel_Bateria / 4);
    Sensores.Nivel_Bateria = (DWORD) ((Sensores.Nivel_Bateria * 3300) / 1024);
}

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                M�TODOS P�BLICOS
//********************************************************************************************************************    
//********************************************************************************************************************    

//Funci�n que realiza la configuraci�n del conversor y termina inicializando

void ADC_Configura_Inicia(void) {
    //Inicializa variables miembro Sensores y Bateria
    Sensores.Temperatura = 0;
    Sensores.Pluviometria = 0;
    Sensores.Vel_Aire = 0;
    Sensores.Nivel_Bateria = 0;

    NuevaMedidaTomada = FALSE;

    //////////// Reg. AD1CON1 /////////////////
    AD1CON1 = 0;
    _FORM = 0; //Formato dato convertido: Integer
    _SSRC = 0b111; //Auto-Convert -> Inicio autom�tico de la conversi�n
    _ASAM = 1; //Auto-Sample  -> Inicio autom�tico del muestreo

    //////////// Reg. AD1CON2 /////////////////
    AD1CON2 = 0;
    _VCFG = 0; //Voltaje de referencia Avdd y Avss (Alimentaci�n del micro)
    _CSCNA = 1; //Scan de entradas activado
    _SMPI = 0b1111; //Activaci�n Interrupci�n cada 16 ciclos de conversion

    //////////// Reg. AD1CON3 /////////////////
    AD1CON3 = 0;
    _ADRC = 0; //Clock derivado del sistema (No del RC interno)
    _SAMC = 0b11111; //Tiempo de muestreo:   31 Tad  (Tad: Periodo conversi�n de 1 bit -> 12 Tad = 1 dato)
    _ADCS = 0b11111111; //Tiempo de conversi�n: 128 Tcy (Tcy: Periodo instrucci�n)

    //////////// Reg. AD1PCFG /////////////////
    AD1PCFG = 0; //Configuraci�n de pines de entrada como anal�gicos.

    //////////// Reg. AD1CSSL /////////////////
    AD1CSSL = 0x0039; //Scan de entradas AN4 y AN5 -> Ubicaci�n del Sensor Temperatura y del Potenci�metro (Pluviometria) respectivamente.
    //Scan de entradas AN3 y AN0 -> Ubicaci�n del Sensor de Velocidad Aire y Nivel de Bater�a
    //Habilitaci�n de interrupci�n y puesta en marcha.
    _AD1IF = 0;
    _AD1IE = 1;
    _ADON = 1;
}


//Funci�n que devuelve el valor de la temperatura, de la pluviometr�a, de la velocidad del aire y de la tensi�n de la bater�a

SENSORES ADC_Lectura_Sensores(void) {
    NuevaMedidaTomada = FALSE;
    return Sensores;
}

//Funci�n que apaga el A/D

void ADC_Detiene(void) {
    _ADON = 0;
}

//Indica si hay una nueva medida lista para su utilizaci�n.

BOOL ADC_HayNuevaMedida(void) {
    return NuevaMedidaTomada;
}


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                INTERRUPCIONES
//********************************************************************************************************************    
//********************************************************************************************************************    

//Rutina de atenci�n a la interrupcci�n. Solamente actualiza el valor de la variable Sensores, que recoge sus valores actuales

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void) {
    Actualiza_Sensores();
    NuevaMedidaTomada = TRUE;
    IFS0bits.AD1IF = 0;
}






