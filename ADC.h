
#ifndef ADC_H
    #define ADC_H

 //////////////////////////////////////////////////////////////    
//INCLUDES
//////////////////////////////////////////////////////////////
#include "General.h"	
	
//////////////////////////////////////////////////////////////    
//METODOS PUBLICOS   Definición de funciones o prototipos
//////////////////////////////////////////////////////////////
void ADC_Configura_Inicia(void);
SENSORES ADC_Lectura_Sensores(void);
void ADC_Detiene(void);
BOOL ADC_HayNuevaMedida(void);

#endif
