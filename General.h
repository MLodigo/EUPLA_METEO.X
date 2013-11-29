/*********************************************************************
 *
 *                  Definición de tipos de variables genéricas
 *
 *********************************************************************
 * Nombre:          General.h
 *
 * Procesador:      PIC24F
 *
 * Compilador:      MPLAB C30 v1.31.00 or higher
 *                  MPLAB IDE v7.22.00 or higher
 *
 * Compañía:        EUPLA
 *
 ********************************************************************/
#ifndef _GENERAL_H_
#define _GENERAL_H_

#define PUBLIC
#define PROTECTED
#define PRIVATE                 static

typedef unsigned char       BYTE;               // 8-bit
typedef unsigned short int  WORD;               // 16-bit
typedef unsigned long       DWORD;              // 32-bit

typedef unsigned int        UINT;
typedef unsigned char       UINT8;              // Otro nombre para 8-bit integer
typedef unsigned short      UINT16;             // Otro nombre para 16-bit integer
typedef unsigned long       UINT32;             // Otro nombre para 32-bit integer

//Tipo definido para un Booleano
typedef enum _BOOL { FALSE = 0, TRUE, ERROR } BOOL;

//Manejo de bits en un byte
typedef struct
{
    BYTE    b0:     1;
    BYTE    b1:     1;
    BYTE    b2:     1;
    BYTE    b3:     1;
    BYTE    b4:     1;
    BYTE    b5:     1;
    BYTE    b6:     1;
    BYTE    b7:     1;

}BYTE_BITS;

//Manejo de bits en un word
typedef struct
{
    WORD    b0:     1;
    WORD    b1:     1;
    WORD    b2:     1;
    WORD    b3:     1;
    WORD    b4:     1;
    WORD    b5:     1;
    WORD    b6:     1;
    WORD    b7:     1;
    WORD    b8:     1;
    WORD    b9:     1;
    WORD    b10:    1;
    WORD    b11:    1;
    WORD    b12:    1;
    WORD    b13:    1;
    WORD    b14:    1;
    WORD    b15:    1;
}WORD_BITS;

//Unión para manejar un byte y sus bits
typedef union _BYTE_VAL
{
    BYTE_BITS bits;
    BYTE Val;
} BYTE_VAL;

//Unión para manejar un word en valor, en bits, byte alto, byte bajo y como array de dos bytes
typedef union _WORD_VAL
{
    WORD Val;
    WORD_BITS   bits;
    struct
    {
        BYTE LB;
        BYTE HB;
    } byte;
    struct
    {
        BYTE_VAL    low;
        BYTE_VAL    high;
    }byteUnion;

    BYTE v[2];
} WORD_VAL;

//Unión para manejar una doble word como valor, como 4 bytes, como 2 words, como array de 4 bytes o como array de 2 words
typedef union _DWORD_VAL
{
    DWORD Val;
    struct
    {
        BYTE LB;
        BYTE HB;
        BYTE UB;
        BYTE MB;
    } byte;
    struct
    {
        WORD LW;
        WORD HW;
    } word;
    struct
    {
        WORD_VAL    low;
        WORD_VAL    high;
    }wordUnion;
    struct
    {
        BYTE_VAL    lowLSB;
        BYTE_VAL    lowMSB;
        BYTE_VAL    highLSB;
        BYTE_VAL    highMSB;
    }byteUnion;
    BYTE v[4];
    WORD w[2];
} DWORD_VAL;

//Macros de manejo de bytes en función de sus peso en la word o doble word
#define LSB(a)          ((a).v[0])
#define MSB(a)          ((a).v[1])

#define LOWER_LSB(a)    ((a).v[0])
#define LOWER_MSB(a)    ((a).v[1])
#define UPPER_LSB(a)    ((a).v[2])
#define UPPER_MSB(a)    ((a).v[3])


//Tipo definido para almacenar las señales meteorológicas que se van a muestrear
typedef struct
{
    WORD    Pluviometria;
    WORD    Temperatura;
    WORD    Vel_Aire;
    WORD    Nivel_Bateria;
}SENSORES;

#endif
