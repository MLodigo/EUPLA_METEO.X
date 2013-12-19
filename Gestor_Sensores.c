
#include "Gestor_Sensores.h"
#include "Gestor_EEPROM.h"
#include "ADC.h"
#include "UART.h"
#include "Reloj_RTCC.h"
#include "SPI_EEPROM.h"

/*NOTAS:

    1.-Una Muestra se compone de medidas.
       Una medida puede ser de temperatura, pluviometr�a, velocidad del aire o nivel de bater�a.

    2.-Inicialmente, la memoria debe de contener todos valores a 0xFF.
       Cuando se aplica una configuraci�n standart, los contadores de M�ximo se inicializan (Despiertes_MAX y Muestras_MAX_Envio_Modem).
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

//********************************************************************************************************************
//Funci�n que obtiene el valor actual del contador de despiertes llevados a cabo por el micro.
//********************************************************************************************************************
WORD Read_CNT_Despiertes()
{
    WORD_VAL Resultado;

    //Lectura en memoria del contador
    Resultado.byte.LB = EEPROM_ReadByte(DIR_CNT_DESPIERTES);
    Resultado.byte.HB = EEPROM_ReadByte(DIR_CNT_DESPIERTES+1);

    //Si el Resultado es 0xFFFF, significa que la memoria fu� inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(Resultado.Val == 0xFFFF)
    {
        Resultado.Val = 0;
    }

    return Resultado.Val;
}

//********************************************************************************************************************
//Funci�n que obtiene el n�mero de despiertes que debe realizar el micro, antes de efectuar una toma de muestra y
//almacenarla en memoria.
//********************************************************************************************************************
WORD Read_Despiertes_MAX()
{
    WORD_VAL Resultado;

    //Lectura en memoria del dato.
    Resultado.byte.LB = EEPROM_ReadByte(DIR_DESPIERTES_POR_MEDIDA);
    Resultado.byte.HB = EEPROM_ReadByte(DIR_DESPIERTES_POR_MEDIDA+1);

    //Si el Resultado es 0xFFFF, significa que la memoria fu� inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(Resultado.Val == 0xFFFF)
    {
        Resultado.Val = 0;
    }

    return Resultado.Val;
}

//********************************************************************************************************************
//Funci�n que obtiene el valor actual del contador de muestras tomadas.
//********************************************************************************************************************
WORD Read_CNT_Muestras_Tomadas()
{
    WORD_VAL Resultado;

    //Lectura en memoria del dato
    Resultado.byte.LB = EEPROM_ReadByte(DIR_CNT_MUESTRAS_TOMADAS);
    Resultado.byte.HB = EEPROM_ReadByte(DIR_CNT_MUESTRAS_TOMADAS+1);

    //Si el Resultado es 0xFFFF, significa que la memoria fu� inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(Resultado.Val == 0xFFFF)
    {
        Resultado.Val = 0;
    }

    return Resultado.Val;

}

//********************************************************************************************************************
//Funci�n que obtiene el valor de muestras m�ximas a acumular en memoria, antes de enviar toda la informaci�n v�a modem.
//********************************************************************************************************************
WORD Read_Muestras_MAX_Envio_Modem()
{
    WORD_VAL Resultado;

    //Lectura en memoria del dato
    Resultado.byte.LB = EEPROM_ReadByte(DIR_MUESTRAS_POR_ENVIO_MODEM);
    Resultado.byte.HB = EEPROM_ReadByte(DIR_MUESTRAS_POR_ENVIO_MODEM+1);

    //Si el Resultado es 0xFFFF, significa que la memoria fu� inicializada y por tanto el valor del contador lo asignaremos a 0.
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
//Funci�n que obtiene la siguiente direcci�n sobre la que se escribir� la siguiente muestra que se tome.
//Se calcula en base al n�mero de muestras tomadas, el tama�o de cada muestra y la direccion de comienzo de
//la zona de almacenamiento de muestras en la memoria EEPROM.
//********************************************************************************************************************
WORD Siguiente_Direccion_Libre_EEPROM()
{
    //WORD_VAL CNT_Muestras_MAX_Por_Envio;
    WORD CNT_Muestras_Tomadas = 0;
    WORD Direccion = 0;

    //Lectura del contador
    CNT_Muestras_Tomadas = Read_CNT_Muestras_Tomadas();

    //Si el contador est� a 0xFFFF, significa que la memoria fu� inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(CNT_Muestras_Tomadas == 0xFFFF)
    {
        CNT_Muestras_Tomadas = 0;
    }

    //C�lculo de la siguiente direcci�n libre
    Direccion = (CNT_Muestras_Tomadas * BYTES_POR_MEDIDA) + DIR_BASE_MEDIDAS;

    //Resultado
    return Direccion;
}

//********************************************************************************************************************
//Funci�n que se apoya en el conversor A/D para realizar un barrido a los sensores y obtener las medidas de cada uno.
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
//Funci�n que realiza el guardado de la muestra pasada como argumento, en la direcci�n indicada.
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
    //Si no se realiz� correctamente, salimos.
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
    //Si no se realiz� correctamente, salimos.
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
    //Si no se realiz� correctamente, salimos.
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
    //Si no se realiz� correctamente, salimos.
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
    //Si no se realiz� correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    //CHECKSUM: DDMMYYHHMMTTTTPPPPVVVVBBBB + CHK
    Suma += FechaHoraReloj.f.DiaMes;
    Suma += FechaHoraReloj.f.Mes;
    Suma += FechaHoraReloj.f.Ano;
    Suma += FechaHoraReloj.f.Hora;
    Suma += FechaHoraReloj.f.Minutos;
    Medida.Val = (WORD)Muestra.Temperatura;
    Suma += Medida.byte.HB;
    Suma += Medida.byte.LB;
    Medida.Val = (WORD)Muestra.Pluviometria;
    Suma += Medida.byte.HB;
    Suma += Medida.byte.LB;
    Medida.Val = (WORD)Muestra.Vel_Aire;
    Suma += Medida.byte.HB;
    Suma += Medida.byte.LB;
    Medida.Val = (WORD)Muestra.Nivel_Bateria;
    Suma += Medida.byte.HB;
    Suma += Medida.byte.LB;
    //Checksum final
    Suma = 0x100 - Suma;

    //Escritura de la suma de comprobaci�n CHECKSUM///////////////
    Reintentos = 0;
    do
    {
        EEPROM_WriteByte(Suma, Direccion+13);
        Reintentos++;
    }
    while((EEPROM_ReadByte(Direccion+13)!=Suma)&&(Reintentos<3));
    //Si no se realiz� correctamente, salimos.
    if(Reintentos==3){return FALSE;}

    //Si se lleg� aqu�, es que todo el proceso fu� correcto.
    return TRUE;
}

//********************************************************************************************************************
//Funci�n que informa si ha llegado o no, el momento de hacer un barrido a los sensores con el objetivo de almacenar
//la informaci�n recogida en memoria EEPROM.
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

    //Lectura del n�mero de despiertes antes de tomar una muestra
    Reintentos=0;
    do
    {
        Cnt_Despiertes_MAX = Read_Despiertes_MAX();
        Reintentos++;
    }while((Cnt_Despiertes_MAX==0xFFFF)&&(Reintentos<3));

    if(Reintentos==3){return FALLO;}

    //Valoraci�n final
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
//Funci�n que informa se ha llegado o no, el momento de realizar un envio v�a modem al centro servidor, de todos las medidas
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

    //Lectura del n�mero de despiertes antes de tomar una muestra
    Reintentos=0;
    do
    {
        Cnt_Muestras_MAX = Read_Muestras_MAX_Envio_Modem();
        Reintentos++;
    }while((Cnt_Muestras_MAX==0xFFFF)&&(Reintentos<3));

    if(Reintentos==3){return FALLO;}

    //Valoraci�n final
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
//Funci�n que realiza el env�o de toda la informaci�n almacenada en memoria, v�a modem al centro servidor.
//********************************************************************************************************************
BOOL Enviar_Muestras_Modem()
{
    WORD cntDireccion=0;
    WORD DirFinal = 0;

    DirFinal = Siguiente_Direccion_Libre_EEPROM();

    //Habilitamos la UART
    UART2_Configura_Abre();

    //Lectura de cada uno de los datos de la memoria y env�o por la UART
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
//Funci�n que incrementa en uno, el contador que recoge las veces que el micro se ha despertado, almacen�ndolo en EEPROM.
//********************************************************************************************************************
BOOL Inc_CNT_Despiertes()
{
    WORD_VAL Resultado;
    BYTE Reintentos = 0;

    //Lectura del valor actual e incremento.
    Resultado.Val = (WORD)(Read_CNT_Despiertes()+1);

    //Actualizaci�n del valor en memoria
    do
    {
        EEPROM_WriteByte(Resultado.byte.LB, DIR_CNT_DESPIERTES);
        EEPROM_WriteByte(Resultado.byte.HB, DIR_CNT_DESPIERTES+1);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(DIR_CNT_DESPIERTES)!=Resultado.byte.LB)||(EEPROM_ReadByte(DIR_CNT_DESPIERTES+1)!=Resultado.byte.HB))&&(Reintentos<3));

    //Comprobaci�n final..
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
//Funci�n que incrementa en uno, el contador que recoge el total de muestras tomadas, almacen�ndolo en EEPROM.
//********************************************************************************************************************
BOOL Inc_CNT_Muestras_Tomadas()
{
    WORD_VAL Resultado;
    BYTE Reintentos = 0;

    //Lectura del valor actual e incremento de una unidad.
    Resultado.Val = (WORD)(Read_CNT_Muestras_Tomadas()+1);

    //Actualilzaci�n del valor en memoria.
    do
    {
        EEPROM_WriteByte(Resultado.byte.LB, DIR_CNT_MUESTRAS_TOMADAS);
        EEPROM_WriteByte(Resultado.byte.HB, DIR_CNT_MUESTRAS_TOMADAS+1);
        Reintentos++;
    }
    while(((EEPROM_ReadByte(DIR_CNT_MUESTRAS_TOMADAS)!=Resultado.byte.LB)||(EEPROM_ReadByte(DIR_CNT_MUESTRAS_TOMADAS+1)!=Resultado.byte.HB))&&(Reintentos<3));

    //Comprobaci�n final..
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
//Funci�n que pone a cero el contador de veces que el micro se ha despertado.
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

    //Comprobaci�n final..
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
//Funci�n que pone a cero el contador de muestras tomadas.
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

    //Comprobaci�n final..
    if(Reintentos==3)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
