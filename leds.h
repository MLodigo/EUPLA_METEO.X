/******************************************************************************
 *
 *                  FICHERO HEADER PARA MANEJO DE LED'S
 *
 ******************************************************************************
 * Nombre:          led.h
 * Procesador:      PIC24
 * Compilador:      MPLAB C30
 * Compañia:        Microchip Technology, Inc.
 *****************************************************************************/
#ifndef leds_h
    #define leds_h

    //Macros de pines I/O donde están conectados los LED's
    #define LED3			LATAbits.LATA0
    #define LED4			LATAbits.LATA1
    #define LED5			LATAbits.LATA2
    #define LED6			LATAbits.LATA3
    #define LED7			LATAbits.LATA4
    #define LED8			LATAbits.LATA5
    #define LED9			LATAbits.LATA6
    #define LED10			LATAbits.LATA7
    
    //Macros para el control de dirección de pines I/O donde están conectados los LED's
    #define LED3_TRIS		TRISAbits.TRISA0
    #define LED4_TRIS		TRISAbits.TRISA1
    #define LED5_TRIS		TRISAbits.TRISA2
    #define LED6_TRIS		TRISAbits.TRISA3
    #define LED7_TRIS		TRISAbits.TRISA4
    #define LED8_TRIS		TRISAbits.TRISA5
    #define LED9_TRIS		TRISAbits.TRISA6			
    #define LED10_TRIS		TRISAbits.TRISA7
    
    //Macros para encender y apagar LED's
    #define LED3_ON LED3_TRIS=0; LED3=1;
    #define LED3_OFF LED3_TRIS=0; LED3=0;
    #define LED4_ON LED4_TRIS=0; LED4=1;
    #define LED4_OFF LED4_TRIS=0; LED4=0;
    #define LED5_ON LED5_TRIS=0; LED5=1;
    #define LED5_OFF LED5_TRIS=0; LED5=0;
    #define LED6_ON LED6_TRIS=0; LED6=1;
    #define LED6_OFF LED6_TRIS=0; LED6=0;
    #define LED7_ON LED7_TRIS=0; LED7=1;
    #define LED7_OFF LED7_TRIS=0; LED7=0;
    #define LED8_ON LED8_TRIS=0; LED8=1;
    #define LED8_OFF LED8_TRIS=0; LED8=0;
    #define LED9_ON LED9_TRIS=0; LED9=1;
    #define LED9_OFF LED9_TRIS=0; LED9=0;
    #define LED10_ON LED10_TRIS=0; LED10=1;
    #define LED10_OFF LED10_TRIS=0; LED10=0;

#define LEDs_ON LED3_ON;LED4_ON;LED5_ON;LED6_ON;LED7_ON;LED8_ON;LED9_ON;LED10_ON;
    #define LEDs_OFF LED3_OFF;LED4_OFF;LED5_OFF;LED6_OFF;LED7_OFF;LED8_OFF;LED9_OFF;LED10_OFF;
    
    //Macros para consulta de estado de los LED's
    #define IS_LED3_ON			PORTAbits.RA0==1
    #define IS_LED4_ON			PORTAbits.RA1==1
    #define IS_LED5_ON			PORTAbits.RA2==1
    #define IS_LED6_ON			PORTAbits.RA3==1
    #define IS_LED7_ON			PORTAbits.RA4==1
    #define IS_LED8_ON			PORTAbits.RA5==1
    #define IS_LED9_ON			PORTAbits.RA6==1
    #define IS_LED10_ON			PORTAbits.RA7==1
#endif


