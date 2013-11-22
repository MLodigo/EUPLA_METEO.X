/*

  inclusión de librerías
  ...
  definición de macros y constantes
  ...
  definiciones de funciones o/y prototipos
  ...
  definiciones de variables globales
  ...
*/

#include "General.h"

#ifndef UART_H
    #define UART_H

 

//////////////////////////////////////////////////////////////    
//METODOS PUBLICOS   Definición de funciones o prototipos
//////////////////////////////////////////////////////////////
void UART2_Configura_Abre(void);
UINT8 UART2_DatosPorLeer(void);
UINT8 UART2_LeerDatos(char *buffer, UINT8 numDatos);
void UART2_Cierra(void);
BOOL UART2_Hay_Sobrescritura_Buffer_RX(void);
void UART2_Envia_Byte(unsigned char dato);
void UART2_Envia_Cadena(char *cadena);
void UART2_Configura_Modo_Sleep(void);

#endif


