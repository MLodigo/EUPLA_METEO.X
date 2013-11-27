#include "ADC.h"
#include <p24fj128ga010.h>
#include "General.h"

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                VARIABLES MIEMBRO
//********************************************************************************************************************    
//********************************************************************************************************************    
PRIVATE SENSORES Sensores; //Guarda la última medida de los valores de los sensores y bateria
PRIVATE BOOL NuevaMedidaTomada = FALSE; //Indica si hay nuevas medidas que no se hayan leido todavía

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                MÉTODOS PRIVADOS
//********************************************************************************************************************    
//********************************************************************************************************************    
void Actualiza_Sensores(void);

//********************************************************************************************************************
//Función que mantiene actualizada las variables Sensores y Batería tras cada medida realizada por el conversor ADC
//********************************************************************************************************************
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

    //Medida de Estado Batería
    Sensores.Nivel_Bateria = ADC1BUF0 + ADC1BUF4 + ADC1BUF8 + ADC1BUFC;
    Sensores.Nivel_Bateria = (DWORD) (Sensores.Nivel_Bateria / 4);
    Sensores.Nivel_Bateria = (DWORD) ((Sensores.Nivel_Bateria * 3300) / 1024);
}

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                MÉTODOS PÚBLICOS
//********************************************************************************************************************    
//********************************************************************************************************************    

//********************************************************************************************************************
//Función que realiza la configuración del conversor y termina inicializandolo.
//********************************************************************************************************************
void ADC_Configura_Inicia(void) {
    //Inicializa variables miembro Sensores y Bateria
    Sensores.Temperatura = 0;
    Sensores.Pluviometria = 0;
    Sensores.Vel_Aire = 0;
    Sensores.Nivel_Bateria = 0;

    //Bajamos la bandera indicativa de que hay una nueva medida por leer.
    NuevaMedidaTomada = FALSE;

    //////////// Reg. AD1CON1 /////////////////
    AD1CON1 = 0;
    _FORM = 0;      //Formato dato convertido: Integer
    _SSRC = 0b111;  //Auto-Convert -> Inicio automático de la conversión
    _ASAM = 1;      //Auto-Sample  -> Inicio automático del muestreo

    //////////// Reg. AD1CON2 /////////////////
    AD1CON2 = 0;
    _VCFG = 0;      //Voltaje de referencia Avdd y Avss (Alimentación del micro)
    _CSCNA = 1;     //Scan de entradas activado
    _SMPI = 0b1111; //Activación Interrupción cada 16 ciclos de conversion

    //////////// Reg. AD1CON3 /////////////////
    AD1CON3 = 0;
    _ADRC = 0;          //Clock derivado del sistema (No del RC interno)
    _SAMC = 0b11111;    //Tiempo de muestreo:   31 Tad  (Tad: Periodo conversión de 1 bit -> 12 Tad = 1 dato)
    _ADCS = 0b11111111; //Tiempo de conversión: 128 Tcy (Tcy: Periodo instrucción)

    //////////// Reg. AD1PCFG /////////////////
    AD1PCFG = 0; //Configuración de pines de entrada como analógicos.

    //////////// Reg. AD1CSSL /////////////////
    AD1CSSL = 0x0039; //Scan de entradas AN4 y AN5 -> Ubicación del Sensor Temperatura y del Potenciómetro (Pluviometria) respectivamente.
                      //Scan de entradas AN3 y AN0 -> Ubicación del Sensor de Velocidad Aire y Nivel de Batería
    
    //Habilitación de interrupción y puesta en marcha
    _AD1IF = 0;
    _AD1IE = 1;
    _ADON = 1;
}

//********************************************************************************************************************
//Función que devuelve el valor de la temperatura, de la pluviometría, de la velocidad del aire y de la tensión de la batería.
//********************************************************************************************************************
SENSORES ADC_Lectura_Sensores(void) {
    NuevaMedidaTomada = FALSE;
    return Sensores;
}

//********************************************************************************************************************
//Función que apaga el conversor A/D.
//********************************************************************************************************************
void ADC_Detiene(void) {
    _ADON = 0;
}

//********************************************************************************************************************
//Función que indica si hay una nueva medida lista para su utilización.
//********************************************************************************************************************
BOOL ADC_HayNuevaMedida(void) {
    return NuevaMedidaTomada;
}


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                INTERRUPCIONES
//********************************************************************************************************************    
//********************************************************************************************************************    

//********************************************************************************************************************
//Rutina de atención a la interrupcción. Tras cada proceso de conversión, se actualiza el valor de la variable Sensore
//********************************************************************************************************************s.
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void) {
    Actualiza_Sensores();
    NuevaMedidaTomada = TRUE;
    IFS0bits.AD1IF = 0;
}
