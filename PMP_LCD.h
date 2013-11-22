#ifndef PMP_LCD_H
    #define PMP_LCD_H

 //////////////////////////////////////////////////////////////    
//INCLUDES
//////////////////////////////////////////////////////////////
#include "General.h"

//////////////////////////////////////////////////////////////    
//DEFINES
//////////////////////////////////////////////////////////////

//Número de carácteres por línea del display
#define LCD_DISPLAY_LEN 	16	
	
//Número de líneas del display
#define LCD_DISPLAY_LINE	2		

#define LCD_LINEA1			0		//Primera línea
#define LCD_LINEA2			1		//Segunda línea

//Diferencia entre escribir una instrucción o un dato en el display
#define LCD_I				0		//Intrucción
#define LCD_D				1		//Dato

#define LCD_Busy			0x80	//LCD ocupado
#define LCD_Free			0		//LCD libre 

//COMANDOS DISPLAY 

//Borra módulo DDRAM(Pantalla). Cursor en posición 0. Bit I/D en Incremento (Automático de direcciones)
#define LCD_CLEAR_DISPLAY				0x01	
//Cursor al inicio (Direccion 0). No se modifica DDRAM. Registro contador direcciones (AC) puesto a 0. 
#define LCD_RETURN_HOME					0x02	
//Movimiento cursor Incremental y la pantalla no se desplaza (No shift)
#define LCD_CURSOR_INC_NS				0x06	
//Movimiento cursor Decremental y la pantalla no se desplaza (No shift)
#define LCD_CURSOR_DEC_NS				0x04	
//Movimiento cursor Incremental y la pantalla se desplaza (Shift)
#define LCD_CURSOR_INC_S				0x07	
//Movimiento cursor Decremental y la pantalla se desplaza (Shift)
#define LCD_CURSOR_DEC_S				0x05	
//Display ON, Cursor ON, Parpadeo del cursor OFF
#define LCD_DIS_ON_CUR_ON_PAR_OFF		0x0e	
//Display ON, Cursor ON, Parpadeo del cursor ON
#define LCD_DIS_ON_CUR_ON_PAR_ON		0x0f	
//Display ON, Cursor OFF, Parpadeo del cursor OFF
#define LCD_DIS_ON_CUR_OFF_PAR_OFF		0x0c	
//Display OFF, Cursor OFF, Parpadeo del cursor OFF
#define LCD_DIS_OFF_CUR_OFF_PAR_OFF		0x08	
//Desplazamiento del cursor activado y a la izquierda
#define LCD_CURSOR_SHIFT_LEFT			0x10	
//Desplazamiento del cursor activado y a la derecha
#define LCD_CURSOR_SHIFT_RIGHT			0x14	
//Desplazamiento del cursor deshabilitado
#define LCD_CURSOR_NO_SHIFT				0x18	
//Longitud de datos de 4-bit, LCD 1 Línea, Caracteres 5x7
#define LCD_DATOS_4_BITS				0x20	
//Longitud de datos de 8-bit, LCD 2 Líneas, Caracteres 5x10
#define LCD_DATOS_8_BITS				0x3C	
//Direccion de CGRAM de 4 bits
#define LCD_CGRAM(_cgramAddr)	(((_cgramAddr)&0x0f)|0x40)		
//Direccion de DDRAM Linea 1 de 7 bits
#define LCD_DDRAM1(_ddramAddr)	(((_ddramAddr)&0x0f)|0x80)		
//Direccion de DDRAM Linea 2 de 7 bits
#define LCD_DDRAM2(_ddramAddr)	(((_ddramAddr)&0x0f)|0xc0)		

//////////////////////////////////////////////////////////////    
//MACROS
//////////////////////////////////////////////////////////////
#define LCD_Ocupado() LCD_Read(LCD_I) & 0x80

//////////////////////////////////////////////////////////////    
//CONSTANTES
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////    
//TIPOS DEFINIDOS Y ESTRUCTURAS
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////    
//METODOS PUBLICOS   Definición de funciones o prototipos
//////////////////////////////////////////////////////////////
void LCD_Inicializacion();
void LCD_WriteLinea(unsigned char numLinea, unsigned char * cadenaDisplay);
void LCD_WrString_LinPos(int numLinea, int posicion, unsigned char *cadena);
void LCD_Clear();

#endif
