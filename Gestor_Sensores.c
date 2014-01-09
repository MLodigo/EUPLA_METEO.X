
#include "Gestor_Sensores.h"
#include "Gestor_EEPROM.h"
#include "ADC.h"
#include "UART.h"
#include "Reloj_RTCC.h"
#include "SPI_EEPROM.h"
#include "string.h"
#include <stdio.h>


/*NOTAS:

    1.-Una Muestra se compone de medidas.
       Una medida puede ser de temperatura, pluviometría, velocidad del aire o nivel de batería.

    2.-Inicialmente, la memoria debe de contener todos valores a 0xFF.
       Cuando se aplica una configuración standart, los contadores de Máximo se inicializan (Despiertes_MAX y Muestras_MAX_Envio_Modem).
       Cuando se hace una lectura, si el contador se lee 0xFFFF, se interpreta como no inicializado (con valor 0).
*/


//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PRIVADOS
//********************************************************************************************************************
//********************************************************************************************************************

//Prototipos
WORD Read_CNT_Despiertes();
WORD Read_Despiertes_MAX();
WORD Read_CNT_Muestras_Tomadas();
WORD Read_Muestras_MAX_Envio_Modem();
void Formatear_Muestra_SvrSplunk(BYTE*,rtccFechaHora,SENSORES);
BYTE Calcula_Cheksum_Muestra(rtccFechaHora, SENSORES);

//********************************************************************************************************************
//Función que obtiene el valor actual del contador de despiertes llevados a cabo por el micro.
//********************************************************************************************************************
WORD Read_CNT_Despiertes()
{
    WORD_VAL Resultado;

    //Lectura en memoria del contador
    Resultado.byte.LB = EEPROM_ReadByte(DIR_CNT_DESPIERTES);
    Resultado.byte.HB = EEPROM_ReadByte(DIR_CNT_DESPIERTES+1);

    //Si el Resultado es 0xFFFF, significa que la memoria fué inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(Resultado.Val == 0xFFFF)
    {
        Resultado.Val = 0;
    }

    return Resultado.Val;
}

//********************************************************************************************************************
//Función que obtiene el número de despiertes que debe realizar el micro, antes de efectuar una toma de muestra y
//almacenarla en memoria.
//********************************************************************************************************************
WORD Read_Despiertes_MAX()
{
    WORD_VAL Resultado;

    //Lectura en memoria del dato.
    Resultado.byte.LB = EEPROM_ReadByte(DIR_DESPIERTES_POR_MEDIDA);
    Resultado.byte.HB = EEPROM_ReadByte(DIR_DESPIERTES_POR_MEDIDA+1);

    //Si el Resultado es 0xFFFF, significa que la memoria fué inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(Resultado.Val == 0xFFFF)
    {
        Resultado.Val = 0;
    }

    return Resultado.Val;
}

//********************************************************************************************************************
//Función que obtiene el valor actual del contador de muestras tomadas.
//********************************************************************************************************************
WORD Read_CNT_Muestras_Tomadas()
{
    WORD_VAL Resultado;

    //Lectura en memoria del dato
    Resultado.byte.LB = EEPROM_ReadByte(DIR_CNT_MUESTRAS_TOMADAS);
    Resultado.byte.HB = EEPROM_ReadByte(DIR_CNT_MUESTRAS_TOMADAS+1);

    //Si el Resultado es 0xFFFF, significa que la memoria fué inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(Resultado.Val == 0xFFFF)
    {
        Resultado.Val = 0;
    }

    return Resultado.Val;

}

//********************************************************************************************************************
//Función que obtiene el valor de muestras máximas a acumular en memoria, antes de enviar toda la información vía modem.
//********************************************************************************************************************
WORD Read_Muestras_MAX_Envio_Modem()
{
    WORD_VAL Resultado;

    //Lectura en memoria del dato
    Resultado.byte.LB = EEPROM_ReadByte(DIR_MUESTRAS_POR_ENVIO_MODEM);
    Resultado.byte.HB = EEPROM_ReadByte(DIR_MUESTRAS_POR_ENVIO_MODEM+1);

    //Si el Resultado es 0xFFFF, significa que la memoria fué inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(Resultado.Val == 0xFFFF)
    {
        Resultado.Val = 0;
    }

    return Resultado.Val;
}

//********************************************************************************************************************
//Función que devuelve una cadena formateada para enviar la muestra al servidor Splunk.
//Ejemplo de resultado final: 2013-11-02 12:15:00,Temperatura=24.0c,Pluviometria=0mm,Vel_Aire=0km/h,Nivel_Bat=58%
//********************************************************************************************************************
void Formatear_Muestra_SvrSplunk(BYTE* ptrCadenaSplunk, rtccFechaHora timestamp, SENSORES muestra)
{
    char Informacion[16];
    BYTE i=0;

    *ptrCadenaSplunk = '2';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = '0';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = (BYTE)((((timestamp.f.Ano)&0xF0)>>4)+0x30);
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = (BYTE)((timestamp.f.Ano&0x0F)+0x30);
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = '-';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = (BYTE)((((timestamp.f.Mes)&0xF0)>>4)+0x30);
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = (BYTE)((timestamp.f.Mes&0x0F)+0x30);
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = '-';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = (BYTE)((((timestamp.f.DiaMes)&0xF0)>>4)+0x30);
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = (BYTE)((timestamp.f.DiaMes&0x0F)+0x30);
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = ' ';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = (BYTE)((((timestamp.f.Hora)&0xF0)>>4)+0x30);
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = (BYTE)((timestamp.f.Hora&0x0F)+0x30);
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = ':';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = (BYTE)((((timestamp.f.Minutos)&0xF0)>>4)+0x30);
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = (BYTE)((timestamp.f.Minutos&0x0F)+0x30);
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = ':';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = '0';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = '0';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = ',';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'T';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'e';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'm';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'p';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'e';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'r';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'a';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 't';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'u';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'r';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'a';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = '=';
    ptrCadenaSplunk ++;

    sprintf(Informacion, "%d", muestra.Temperatura);
    for(i=0; i<16; i++)
    {
        if(Informacion[i]!='\0')
        {
            *ptrCadenaSplunk = (unsigned char)Informacion[i];
            ptrCadenaSplunk++;
        }
        else
        {
            *ptrCadenaSplunk = *(ptrCadenaSplunk-1);
            *(ptrCadenaSplunk-1) = '.';
            ptrCadenaSplunk++;
            break;
        }
    }

    *ptrCadenaSplunk = 'c';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = ',';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'P';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'l';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'u';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'v';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'i';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'o';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'm';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'e';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 't';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'r';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'i';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'a';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = '=';
    ptrCadenaSplunk ++;

    sprintf(Informacion, "%d", muestra.Pluviometria);
    for(i=0; i<16; i++)
    {
        if(Informacion[i]!='\0')
        {
            *ptrCadenaSplunk = (unsigned char)Informacion[i];
            ptrCadenaSplunk++;
        }
        else{break;}
    }

    *ptrCadenaSplunk = 'm';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'm';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = ',';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'V';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'e';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'l';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = '_';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'A';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'i';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'r';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'e';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = '=';
    ptrCadenaSplunk ++;

    sprintf(Informacion, "%d", muestra.Vel_Aire);
    for(i=0; i<16; i++)
    {
        if(Informacion[i]!='\0')
        {
            *ptrCadenaSplunk = (unsigned char)Informacion[i];
            ptrCadenaSplunk++;
        }
        else{break;}
    }

    *ptrCadenaSplunk = 'k';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'm';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = '/';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'h';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = ',';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'N';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'i';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'v';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'e';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'l';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = '_';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'B';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 'a';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = 't';
    ptrCadenaSplunk ++;
    *ptrCadenaSplunk = '=';
    ptrCadenaSplunk ++;

    sprintf(Informacion, "%d", muestra.Nivel_Bateria);
    for(i=0; i<16; i++)
    {
        if(Informacion[i]!='\0')
        {
            *ptrCadenaSplunk = (unsigned char)Informacion[i];
            ptrCadenaSplunk++;
        }
        else{break;}
    }
    *ptrCadenaSplunk = '%';
    ptrCadenaSplunk++;

    *ptrCadenaSplunk = ',';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = 'I';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = 'D';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = '=';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = '1';
    ptrCadenaSplunk++;
    
    //Salto de línea///////////////////////////
    *ptrCadenaSplunk = '\r';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = '\n';
    ptrCadenaSplunk++;
    *ptrCadenaSplunk = '\0';
}

//********************************************************************************************************************
//Función que calcula la checksum complemento A2 resultante de tomar una muestra en un determinado momento del tiempo.
//********************************************************************************************************************
BYTE Calcula_Cheksum_Muestra(rtccFechaHora fechaHoraReloj, SENSORES muestra)
{
    BYTE Suma = 0;
    WORD_VAL Medida;

    //CHECKSUM: DDMMYYHHMMTTTTPPPPVVVVBBBB + CHK
    Suma += fechaHoraReloj.f.DiaMes;
    Suma += fechaHoraReloj.f.Mes;
    Suma += fechaHoraReloj.f.Ano;
    Suma += fechaHoraReloj.f.Hora;
    Suma += fechaHoraReloj.f.Minutos;
    Medida.Val = (WORD)muestra.Temperatura;
    Suma += Medida.byte.HB;
    Suma += Medida.byte.LB;
    Medida.Val = (WORD)muestra.Pluviometria;
    Suma += Medida.byte.HB;
    Suma += Medida.byte.LB;
    Medida.Val = (WORD)muestra.Vel_Aire;
    Suma += Medida.byte.HB;
    Suma += Medida.byte.LB;
    Medida.Val = (WORD)muestra.Nivel_Bateria;
    Suma += Medida.byte.HB;
    Suma += Medida.byte.LB;
    //Checksum final
    Suma = 0x100 - Suma;

    return Suma;
}

//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PUBLICOS
//********************************************************************************************************************
//********************************************************************************************************************

//********************************************************************************************************************
//Función que obtiene la siguiente dirección sobre la que se escribirá la siguiente muestra que se tome.
//Se calcula en base al número de muestras tomadas, el tamaño de cada muestra y la direccion de comienzo de
//la zona de almacenamiento de muestras en la memoria EEPROM.
//********************************************************************************************************************
WORD Siguiente_Direccion_Libre_EEPROM()
{
    //WORD_VAL CNT_Muestras_MAX_Por_Envio;
    WORD CNT_Muestras_Tomadas = 0;
    WORD Direccion = 0;

    //Lectura del contador
    CNT_Muestras_Tomadas = Read_CNT_Muestras_Tomadas();

    //Si el contador está a 0xFFFF, significa que la memoria fué inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(CNT_Muestras_Tomadas == 0xFFFF)
    {
        CNT_Muestras_Tomadas = 0;
    }

    //Cálculo de la siguiente dirección libre
    Direccion = (CNT_Muestras_Tomadas * BYTES_POR_MEDIDA) + DIR_BASE_MEDIDAS;

    //Resultado
    return Direccion;
}

//********************************************************************************************************************
//Función que se apoya en el conversor A/D para realizar un barrido a los sensores y obtener las medidas de cada uno.
//********************************************************************************************************************
//SENSORES Tomar_Muestra_Sensores()
SENSORES Barrido_Sensores()
{
    SENSORES Muestra;
    BYTE Reintentos = 0;

    //Inicializamos variable Muestra
    Muestra.Temperatura = 0xFFFF;
    Muestra.Pluviometria = 0xFFFF;
    Muestra.Vel_Aire = 0xFFFF;
    Muestra.Nivel_Bateria = 0xFFFF;

    //Lectura Sensores
    do
    {
        ADC_Configura_Inicia();
        while(!ADC_HayNuevaMedida());
        Muestra = ADC_Lectura_Sensores();
        ADC_Detiene();
        Reintentos++;
    }
    while(((Muestra.Temperatura==0xFFFF)||(Muestra.Pluviometria==0xFFFF)||(Muestra.Vel_Aire==0xFFFF)||(Muestra.Nivel_Bateria==0xFFFF))&&(Reintentos<3));

    return Muestra;
}

//********************************************************************************************************************
//Función que realiza el guardado de la muestra pasada como argumento, en la dirección indicada.
//Se guarda el timestamp asociado a la muestra.
//********************************************************************************************************************
BOOL Guarda_Muestra_EEPROM(SENSORES Muestra, WORD Direccion)
{
    WORD_VAL Medida;
    BYTE Reintentos = 0;
    BYTE Suma = 0;

    //Escritura de la TIMESTAMP///////////////////////
    rtccFechaHora FechaHoraReloj = Lectura_FechaHora_Reloj();
    Reintentos = 0;
    do
    {
        EEPROM_WriteByte(FechaHoraReloj.f.DiaMes, Direccion);
        EEPROM_WriteByte(FechaHoraReloj.f.Mes, Direccion+1);
        EEPROM_WriteByte(FechaHoraReloj.f.Ano, Direccion+2);
        EEPROM_WriteByte(FechaHoraReloj.f.Hora, Direccion+3);
        EEPROM_WriteByte(FechaHoraReloj.f.Minutos, Direccion+4);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(Direccion)!=FechaHoraReloj.f.DiaMes)||(EEPROM_ReadByte(Direccion+1)!=FechaHoraReloj.f.Mes)||
           (EEPROM_ReadByte(Direccion+2)!=FechaHoraReloj.f.Ano)||(EEPROM_ReadByte(Direccion+3)!=FechaHoraReloj.f.Hora)||
           (EEPROM_ReadByte(Direccion+4)!=FechaHoraReloj.f.Minutos))&&(Reintentos<3));
    //Si no se realizó correctamente, salimos.
    if(Reintentos==3){return FALSE;}


    //Escritura de la TEMPERATURA///////////////////////
    Reintentos = 0;
    do
    {
        Medida.Val = (WORD)Muestra.Temperatura;
        EEPROM_WriteByte(Medida.byte.LB, Direccion+5);
        EEPROM_WriteByte(Medida.byte.HB, Direccion+6);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(Direccion+5)!=Medida.byte.LB)||(EEPROM_ReadByte(Direccion+6)!=Medida.byte.HB))&&(Reintentos<3));
    //Si no se realizó correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    //Escritura de la PLUVIOMETRIA/////////////////////
    Reintentos = 0;
    do
    {
        Medida.Val = (WORD)Muestra.Pluviometria;
        EEPROM_WriteByte(Medida.byte.LB, Direccion+7);
        EEPROM_WriteByte(Medida.byte.HB, Direccion+8);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(Direccion+7)!=Medida.byte.LB)||(EEPROM_ReadByte(Direccion+8)!=Medida.byte.HB))&&(Reintentos<3));
    //Si no se realizó correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    //Escritura de la VELOCIDAD DEL AIRE///////////////
    Reintentos = 0;
    do
    {
        Medida.Val = (WORD)Muestra.Vel_Aire;
        EEPROM_WriteByte(Medida.byte.LB, Direccion+9);
        EEPROM_WriteByte(Medida.byte.HB, Direccion+10);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(Direccion+9)!=Medida.byte.LB)||(EEPROM_ReadByte(Direccion+10)!=Medida.byte.HB))&&(Reintentos<3));
    //Si no se realizó correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    //Escritura del NIVEL DEL LA BATERIA///////////////
    Reintentos = 0;
    do
    {
        Medida.Val = (WORD)Muestra.Nivel_Bateria;
        EEPROM_WriteByte(Medida.byte.LB, Direccion+11);
        EEPROM_WriteByte(Medida.byte.HB, Direccion+12);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(Direccion+11)!=Medida.byte.LB)||(EEPROM_ReadByte(Direccion+12)!=Medida.byte.HB))&&(Reintentos<3));
    //Si no se realizó correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    //CHECKSUM: DDMMYYHHMMTTTTPPPPVVVVBBBB + CHK
    Suma = Calcula_Cheksum_Muestra(FechaHoraReloj, Muestra);

    //Escritura de la suma de comprobación CHECKSUM///////////////
    Reintentos = 0;
    do
    {
        EEPROM_WriteByte(Suma, Direccion+13);
        Reintentos++;
    }
    while((EEPROM_ReadByte(Direccion+13)!=Suma)&&(Reintentos<3));
    //Si no se realizó correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    //Si se llegó aquí, es que todo el proceso fué correcto.
    return TRUE;
}

//********************************************************************************************************************
//Función que informa si ha llegado o no, el momento de hacer un barrido a los sensores con el objetivo de almacenar
//la información recogida en memoria EEPROM.
//********************************************************************************************************************
RESPUESTA Si_Realizar_Medidas_Sensores()
{
    WORD Cnt_Despiertes = 0xFFFF;
    WORD Cnt_Despiertes_MAX = 0xFFFF;
    BYTE Reintentos = 0;

    //Lectura del contador de despiertes
    do
    {
        Cnt_Despiertes = Read_CNT_Despiertes();
        Reintentos++;
    }while((Cnt_Despiertes==0xFFFF)&&(Reintentos<3));

    if(Reintentos==3){return FALLO;}

    //Lectura del número de despiertes antes de tomar una muestra
    Reintentos=0;
    do
    {
        Cnt_Despiertes_MAX = Read_Despiertes_MAX();
        Reintentos++;
    }while((Cnt_Despiertes_MAX==0xFFFF)&&(Reintentos<3));

    if(Reintentos==3){return FALLO;}

    //Valoración final
    if(Cnt_Despiertes>=Cnt_Despiertes_MAX)
    {
        return OK;
    }
    else
    {
        return NOK;
    }
}

//********************************************************************************************************************
//Función que informa se ha llegado o no, el momento de realizar un envio vía modem al centro servidor, de todos las medidas
//de sensores realizadas y almacenadas en memoria EEPROM.
//********************************************************************************************************************
RESPUESTA Si_Realizar_Envio_Muestras_Modem()
{
    WORD Cnt_Muestras = 0xFFFF;
    WORD Cnt_Muestras_MAX = 0xFFFF;
    BYTE Reintentos = 0;

    //Lectura del contador de despiertes
    do
    {
        Cnt_Muestras = Read_CNT_Muestras_Tomadas();
        Reintentos++;
    }while((Cnt_Muestras==0xFFFF)&&(Reintentos<3));

    if(Reintentos==3){return FALLO;}

    //Lectura del número de despiertes antes de tomar una muestra
    Reintentos=0;
    do
    {
        Cnt_Muestras_MAX = Read_Muestras_MAX_Envio_Modem();
        Reintentos++;
    }while((Cnt_Muestras_MAX==0xFFFF)&&(Reintentos<3));

    if(Reintentos==3){return FALLO;}

    //Valoración final
    if(Cnt_Muestras>=Cnt_Muestras_MAX)
    {
        return OK;
    }
    else
    {
        return NOK;
    }
}

//********************************************************************************************************************
//Función que realiza el envío de toda la información almacenada en memoria, vía modem al centro servidor.
//********************************************************************************************************************
BOOL Enviar_Muestras_Modem()
{ 
    WORD cntDireccion = 0;
    WORD DirFinal = 0;
    BYTE CadenaSplunk[100];
    rtccFechaHora Timestamp;
    SENSORES Muestra;
    BYTE Checksum = 0;
    BYTE Checksum_calc = 0;

    //Obtenemos el límite hasta donde realizar la lectura de muestras
    DirFinal = Siguiente_Direccion_Libre_EEPROM();
    if(DirFinal> TAM_MEMORIA_EEPROM){return FALSE;}

    //Lectura de cada uno de los datos de la memoria y envío por la UART
    for(cntDireccion=DIR_BASE_MEDIDAS; cntDireccion<DirFinal; cntDireccion = (WORD)(cntDireccion + BYTES_POR_MEDIDA))
    {
        Timestamp.f.DiaMes = EEPROM_ReadByte(cntDireccion);
        Timestamp.f.Mes = EEPROM_ReadByte(cntDireccion+1);
        Timestamp.f.Ano = EEPROM_ReadByte(cntDireccion+2);
        Timestamp.f.Hora = EEPROM_ReadByte(cntDireccion+3);
        Timestamp.f.Minutos = EEPROM_ReadByte(cntDireccion+4);

        Muestra.Temperatura =EEPROM_ReadByte(cntDireccion+5);
        Muestra.Temperatura |=(WORD)(EEPROM_ReadByte(cntDireccion+6)<<8);
        Muestra.Pluviometria =EEPROM_ReadByte(cntDireccion+7);
        Muestra.Pluviometria |=(WORD)(EEPROM_ReadByte(cntDireccion+8)<<8);
        Muestra.Vel_Aire =EEPROM_ReadByte(cntDireccion+9);
        Muestra.Vel_Aire |=(WORD)(EEPROM_ReadByte(cntDireccion+10)<<8);
        Muestra.Nivel_Bateria =EEPROM_ReadByte(cntDireccion+11);
        Muestra.Nivel_Bateria |=(WORD)(EEPROM_ReadByte(cntDireccion+12)<<8);

        Checksum = EEPROM_ReadByte(cntDireccion+13);
        Checksum_calc = Calcula_Cheksum_Muestra(Timestamp, Muestra);
        //Comprobación de la integridad de la muestra almacenada. Sólo si es correcta, se envía.
        if(Checksum == Checksum_calc)
        {
            Formatear_Muestra_SvrSplunk(CadenaSplunk, Timestamp, Muestra);
            UART2_Envia_Cadena((char*)CadenaSplunk);
        }
    }
    return TRUE;
}

//********************************************************************************************************************
//Función que incrementa en uno, el contador que recoge las veces que el micro se ha despertado, almacenándolo en EEPROM.
//********************************************************************************************************************
BOOL Inc_CNT_Despiertes()
{
    WORD_VAL Resultado;
    BYTE Reintentos = 0;

    //Lectura del valor actual e incremento.
    Resultado.Val = (WORD)(Read_CNT_Despiertes()+1);

    //Actualización del valor en memoria
    do
    {
        EEPROM_WriteByte(Resultado.byte.LB, DIR_CNT_DESPIERTES);
        EEPROM_WriteByte(Resultado.byte.HB, DIR_CNT_DESPIERTES+1);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(DIR_CNT_DESPIERTES)!=Resultado.byte.LB)||(EEPROM_ReadByte(DIR_CNT_DESPIERTES+1)!=Resultado.byte.HB))&&(Reintentos<3));

    //Comprobación final..
    if(Reintentos==3)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//********************************************************************************************************************
//Función que incrementa en uno, el contador que recoge el total de muestras tomadas, almacenándolo en EEPROM.
//********************************************************************************************************************
BOOL Inc_CNT_Muestras_Tomadas()
{
    WORD_VAL Resultado;
    BYTE Reintentos = 0;

    //Lectura del valor actual e incremento de una unidad.
    Resultado.Val = (WORD)(Read_CNT_Muestras_Tomadas()+1);

    //Actualilzación del valor en memoria.
    do
    {
        EEPROM_WriteByte(Resultado.byte.LB, DIR_CNT_MUESTRAS_TOMADAS);
        EEPROM_WriteByte(Resultado.byte.HB, DIR_CNT_MUESTRAS_TOMADAS+1);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(DIR_CNT_MUESTRAS_TOMADAS)!=Resultado.byte.LB)||(EEPROM_ReadByte(DIR_CNT_MUESTRAS_TOMADAS+1)!=Resultado.byte.HB))&&(Reintentos<3));

    //Comprobación final..
    if(Reintentos==3)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//********************************************************************************************************************
//Función que pone a cero el contador de veces que el micro se ha despertado.
//********************************************************************************************************************
BOOL Reset_CNT_Despiertes()
{
    WORD_VAL Resultado;
    BYTE Reintentos = 0;

    Resultado.Val = 0;

    //Escritura del valor a 0
    do
    {
        EEPROM_WriteByte(Resultado.byte.LB, DIR_CNT_DESPIERTES);
        EEPROM_WriteByte(Resultado.byte.HB, DIR_CNT_DESPIERTES+1);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(DIR_CNT_DESPIERTES)!=Resultado.byte.LB)||(EEPROM_ReadByte(DIR_CNT_DESPIERTES+1)!=Resultado.byte.HB))&&(Reintentos<3));

    //Comprobación final..
    if(Reintentos==3)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//********************************************************************************************************************
//Función que pone a cero el contador de muestras tomadas.
//********************************************************************************************************************
BOOL Reset_CNT_Muestras_Tomadas()
{
    WORD_VAL Resultado;
    BYTE Reintentos = 0;

    Resultado.Val = 0;

    //Escritura del valor a 0
    do
    {
        EEPROM_WriteByte(Resultado.byte.LB, DIR_CNT_MUESTRAS_TOMADAS);
        EEPROM_WriteByte(Resultado.byte.HB, DIR_CNT_MUESTRAS_TOMADAS+1);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(DIR_CNT_MUESTRAS_TOMADAS)!=Resultado.byte.LB)||(EEPROM_ReadByte(DIR_CNT_MUESTRAS_TOMADAS+1)!=Resultado.byte.HB))&&(Reintentos<3));

    //Comprobación final..
    if(Reintentos==3)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
