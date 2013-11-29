
#include "Gestor_Sensores.h"
#include "Gestor_EEPROM.h"
#include "ADC.h"
#include "UART.h"
#include "Reloj_RTCC.h"

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
WORD Siguiente_Direccion_Libre_EEPROM();
WORD Read_CNT_Despiertes();
WORD Read_Despiertes_MAX();
SENSORES Tomar_Muestra_Sensores();
WORD Read_CNT_Muestras_Tomadas();
WORD Read_Muestras_MAX_Envio_Modem();

//********************************************************************************************************************
//Función que obtiene la siguiente dirección sobre la que se escribirá la siguiente muestra que se tome.
//Se calcula en base al número de muestras tomadas, el tamaño de cada muestra y la direccion de comienzo de
//la zona de almacenamiento de muestras en la memoria EEPROM.
//********************************************************************************************************************
WORD Siguiente_Direccion_Libre_EEPROM()
{
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
    Direccion = (CNT_Muestras_Tomadas *8) + DIR_BASE_MEDIDAS;

    //Resultado
    return Direccion;
}

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
//Función que se apoya en el conversor A/D para realizar un barrido a los sensores y obtener las medidas de cada uno.
//********************************************************************************************************************
SENSORES Tomar_Muestra_Sensores()
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
//********************************************************************************************************************
//                                                METODOS PUBLICOS
//********************************************************************************************************************
//********************************************************************************************************************

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
//Función que inicia un barrido de los sensores, y almacena el resultado (Muestra) en la memoria EEPROM.
//********************************************************************************************************************
BOOL Lectura_Sensores_Guarda_Muestra_EEPROM()
{
    SENSORES Muestra;
    WORD_VAL Medida;
    WORD Direccion = 0;
    BYTE Reintentos = 0;


    //Lectura de Sensores con el conversor A/D
    Muestra = Tomar_Muestra_Sensores();
    if((Muestra.Temperatura==0xFFFF)||(Muestra.Pluviometria==0xFFFF)||(Muestra.Vel_Aire==0xFFFF)||(Muestra.Nivel_Bateria==0xFFFF)){return FALSE;}

    //Obtenemos la dirección a escribir
    Direccion = Siguiente_Direccion_Libre_EEPROM();

    //TODO: Guardado del TimeStamp
    
    //Escritura de la TEMPERATURA///////////////////////
    Reintentos = 0;
    do
    {
        Medida.Val = (WORD)Muestra.Temperatura;
        EEPROM_WriteByte(Medida.byte.LB, Direccion);
        EEPROM_WriteByte(Medida.byte.HB, Direccion+1);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(Direccion)!=Medida.byte.LB)||(EEPROM_ReadByte(Direccion+1)!=Medida.byte.HB))&&(Reintentos<3));
    //Si no se realizó correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    //Escritura de la PLUVIOMETRIA/////////////////////
    Reintentos = 0;
    do
    {
        Medida.Val = (WORD)Muestra.Pluviometria;
        EEPROM_WriteByte(Medida.byte.LB, Direccion+2);
        EEPROM_WriteByte(Medida.byte.HB, Direccion+3);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(Direccion+2)!=Medida.byte.LB)||(EEPROM_ReadByte(Direccion+3)!=Medida.byte.HB))&&(Reintentos<3));
    //Si no se realizó correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    //Escritura de la VELOCIDAD DEL AIRE///////////////
    Reintentos = 0;
    do
    {
        Medida.Val = (WORD)Muestra.Vel_Aire;
        EEPROM_WriteByte(Medida.byte.LB, Direccion+4);
        EEPROM_WriteByte(Medida.byte.HB, Direccion+5);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(Direccion+4)!=Medida.byte.LB)||(EEPROM_ReadByte(Direccion+5)!=Medida.byte.HB))&&(Reintentos<3));
    //Si no se realizó correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    //Escritura del NIVEL DEL LA BATERIA///////////////
    Reintentos = 0;
    do
    {
        Medida.Val = (WORD)Muestra.Nivel_Bateria;
        EEPROM_WriteByte(Medida.byte.LB, Direccion+6);
        EEPROM_WriteByte(Medida.byte.HB, Direccion+7);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(Direccion+6)!=Medida.byte.LB)||(EEPROM_ReadByte(Direccion+7)!=Medida.byte.HB))&&(Reintentos<3));
    //Si no se realizó correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    return TRUE;
}

//********************************************************************************************************************
//Función que realiza el envío de toda la información almacenada en memoria, vía modem al centro servidor.
//********************************************************************************************************************
BOOL Enviar_Muestras_Modem()
{
    WORD cntDireccion=0;
    WORD DirFinal = 0;

    DirFinal = Siguiente_Direccion_Libre_EEPROM();

    //Habilitamos la UART
    UART2_Configura_Abre();

    //Lectura de cada uno de los datos de la memoria y envío por la UART
    for(cntDireccion=DIR_BASE_MEDIDAS; cntDireccion<DirFinal; cntDireccion++)
    {
        UART2_Envia_Byte(EEPROM_ReadByte(cntDireccion));
    }

    //////////////////////////////////////////////////////////////
//    rtccFechaHora FechaHoraReloj = Lectura_FechaHora_Reloj();
//    BYTE FechaFormateada[9];
//    BYTE HoraFormateada[9];
//    FechaFormateada[0] = (BYTE)((((FechaHoraReloj.f.DiaMes) & 0xF0)>>4)+ 0x30);
//    FechaFormateada[1] = (BYTE)(((FechaHoraReloj.f.DiaMes) & 0x0F)+ 0x30);
//    FechaFormateada[2] = '/';
//    FechaFormateada[3] = (BYTE)((((FechaHoraReloj.f.Mes) & 0xF0)>>4)+ 0x30);
//    FechaFormateada[4] = (BYTE)(((FechaHoraReloj.f.Mes) & 0x0F)+ 0x30);
//    FechaFormateada[5] = '/';
//    FechaFormateada[6] = (BYTE)((((FechaHoraReloj.f.Ano) & 0xF0)>>4)+ 0x30);
//    FechaFormateada[7] = (BYTE)(((FechaHoraReloj.f.Ano) & 0x0F)+ 0x30);
//    FechaFormateada[8] = '\0';
//    LCD_WrString_LinPos(LCD_LINEA2, 4, FechaFormateada);
//    HoraFormateada[0] = (BYTE)((((FechaHoraReloj.f.Hora) & 0xF0)>>4)+ 0x30);
//    HoraFormateada[1] = (BYTE)(((FechaHoraReloj.f.Hora) & 0x0F)+ 0x30);
//    HoraFormateada[2] = ':';
//    HoraFormateada[3] = (BYTE)((((FechaHoraReloj.f.Minutos) & 0xF0)>>4)+ 0x30);
//    HoraFormateada[4] = (BYTE)(((FechaHoraReloj.f.Minutos) & 0x0F)+ 0x30);
//    HoraFormateada[5] = ':';
//    HoraFormateada[6] = (BYTE)((((FechaHoraReloj.f.Segundos) & 0xF0)>>4)+ 0x30);
//    HoraFormateada[7] = (BYTE)(((FechaHoraReloj.f.Segundos) & 0x0F)+ 0x30);
//    HoraFormateada[8] = '\0';
//    LCD_WrString_LinPos(LCD_LINEA1, 4, HoraFormateada);
//    Retardo(4000);
    //////////////////////////////////////////////////////////////

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
