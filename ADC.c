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

//********************************************************************************************************************
//Funci�n que mantiene actualizada las variables Sensores y Bater�a tras cada medida realizada por el conversor ADC
//********************************************************************************************************************
void Actualiza_Sensores(void) {
    float Suma = 0;
    float Media = 0.0;
    float Resultado = 0;

    //Media de Temperatura en d�cimas de grado cent�grado.  T�=((valorADC * (3300/1024)) - 500)/10
    Suma = ADC1BUF2 + ADC1BUF6 + ADC1BUFA + ADC1BUFE;
    Media = (Suma / 4);         //Media de las 4 muestras tomadas del sensor
    Resultado = Media * 3300;   //Aplicaci�n de la f�rmula del conversi�n del sensor (Documentaci�n)
    Resultado /=  1024;
    Resultado -=  500;
    Resultado /=  10;
    Resultado -= 6;             //Ajuste
    Resultado *= 10;            //D�cimas de grado
    Sensores.Temperatura = (WORD)Resultado;

    //Media de Pluviometria en litros/m2. Medici�n m�xima 200 litros/m2.
    Suma = ADC1BUF3 + ADC1BUF7 + ADC1BUFB + ADC1BUFF;
    Media = (Suma / 4);
    Resultado = Media * 200;
    Resultado /= 0x3FF;
    Sensores.Pluviometria = (WORD)Resultado;

    //Medida de Velocidad Aire en km/h. Medici�n m�xima 150 km/h.
    Suma = ADC1BUF1 + ADC1BUF5 + ADC1BUF9 + ADC1BUFD;
    Media = (Suma / 4);
    Resultado = Media * 150;
    Resultado /= 0x3FF;
    Sensores.Vel_Aire = (WORD)Resultado;

    //Medida de Estado Bater�a en porcentaje.
    Suma = ADC1BUF0 + ADC1BUF4 + ADC1BUF8 + ADC1BUFC;
    Media = (Suma / 4);
    Resultado = Media * 100;
    Resultado /= 0x3FF;
    Sensores.Nivel_Bateria = (WORD)Resultado;
}

//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                M�TODOS P�BLICOS
//********************************************************************************************************************    
//********************************************************************************************************************    

//********************************************************************************************************************
//Funci�n que realiza la configuraci�n del conversor y termina inicializandolo.
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
    _SSRC = 0b111;  //Auto-Convert -> Inicio autom�tico de la conversi�n
    _ASAM = 1;      //Auto-Sample  -> Inicio autom�tico del muestreo

    //////////// Reg. AD1CON2 /////////////////
    AD1CON2 = 0;
    _VCFG = 0;      //Voltaje de referencia Avdd y Avss (Alimentaci�n del micro)
    _CSCNA = 1;     //Scan de entradas activado
    _SMPI = 0b1111; //Activaci�n Interrupci�n cada 16 ciclos de conversion

    //////////// Reg. AD1CON3 /////////////////
    AD1CON3 = 0;
    _ADRC = 0;          //Clock derivado del sistema (No del RC interno)
    _SAMC = 0b11111;    //Tiempo de muestreo:   31 Tad  (Tad: Periodo conversi�n de 1 bit -> 12 Tad = 1 dato)
    _ADCS = 0b11111111; //Tiempo de conversi�n: 128 Tcy (Tcy: Periodo instrucci�n)

    //////////// Reg. AD1PCFG /////////////////
    AD1PCFG = 0; //Configuraci�n de pines de entrada como anal�gicos.

    //////////// Reg. AD1CSSL /////////////////
    AD1CSSL = 0x0039; //Scan de entradas AN4 y AN5 -> Ubicaci�n del Sensor Temperatura y del Potenci�metro (Pluviometria) respectivamente.
                      //Scan de entradas AN3 y AN0 -> Ubicaci�n del Sensor de Velocidad Aire y Nivel de Bater�a
    
    //Habilitaci�n de interrupci�n y puesta en marcha
    _AD1IF = 0;
    _AD1IE = 1;
    _ADON = 1;
}

//********************************************************************************************************************
//Funci�n que devuelve el valor de la temperatura, de la pluviometr�a, de la velocidad del aire y de la tensi�n de la bater�a.
//********************************************************************************************************************
SENSORES ADC_Lectura_Sensores(void) {
    NuevaMedidaTomada = FALSE;
    return Sensores;
}

//********************************************************************************************************************
//Funci�n que apaga el conversor A/D.
//********************************************************************************************************************
void ADC_Detiene(void) {
    _ADON = 0;
}

//********************************************************************************************************************
//Funci�n que indica si hay una nueva medida lista para su utilizaci�n.
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
//Rutina de atenci�n a la interrupcci�n. Tras cada proceso de conversi�n, se actualiza el valor de la variable Sensore
//********************************************************************************************************************s.
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void) {
    Actualiza_Sensores();
    NuevaMedidaTomada = TRUE;
    IFS0bits.AD1IF = 0;
}
