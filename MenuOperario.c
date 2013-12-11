#include <string.h>
#include <stdio.h>
#include "Menu.h"
#include "MenuOperario.h"
#include "SPI.h"
#include "SPI_EEPROM.h"
#include "PMP_LCD.h"
#include "Switches.h"
#include "Temporizacion.h"
#include "UART.h"
#include "stdio.h"
#include "Gestor_EEPROM.h"


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                VARIABLES MIEMBRO
//********************************************************************************************************************    
//********************************************************************************************************************    

//Indica si la estructura árbol del menú ya fué inicializada
PRIVATE BOOL Estructura_Menu_Inicializada = FALSE;

//Apunta al nodo actualmente seleccionado del menú
PRIVATE ITEM_MENU* NodoActual;

//Nodos del arbol Menu Operário...
PRIVATE ITEM_MENU MenuOp;
PRIVATE ITEM_MENU Configuracion;
PRIVATE ITEM_MENU Muestras;
PRIVATE ITEM_MENU Intervalo;
PRIVATE ITEM_MENU Intervalo_f;
PRIVATE ITEM_MENU Envio;
PRIVATE ITEM_MENU Modem;
PRIVATE ITEM_MENU SmsAlarma;
PRIVATE ITEM_MENU EstadoAlarma;
PRIVATE ITEM_MENU TlfSmsAlarma;
PRIVATE ITEM_MENU PinSIM;
PRIVATE ITEM_MENU EstadoSistema;
PRIVATE ITEM_MENU Memoria;
PRIVATE ITEM_MENU ResetFabrica;
PRIVATE ITEM_MENU EnvioUART;

//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PRIVADOS
//********************************************************************************************************************
//********************************************************************************************************************
//Prototipos de funciones privadas
void Inicializa_Estructura_Menu(void);
void Eje_Interv_Min_Muestras(void);
WORD Read_Despiertes_Muestra(void);
void Eje_Interv_Envio_Muestras(void);
WORD Read_NumMuestras_Envio(void);
void Eje_Config_Envio_SMS_Alarma(void);
BYTE Read_Estado_Envio_SMS_Alarma(void);
void Eje_Config_Tlf_Envio_SMS_Alarma(void);
BOOL Read_Tlf_Envio_Alarma(unsigned char *Numero);
void Eje_Config_PIN_SIM(void);
BOOL Read_PIN_Modem(unsigned char *Numero);
void Eje_Estado_Sistema(void);
BOOL Read_Estado_Sistema(unsigned char *Estado);
void Eje_Reset_Fabrica(void);
BOOL Aplicar_Configuracion_Fabrica();
void Eje_Envio_Mem_UART(void);

//********************************************************************************************************************
//Método que construye la estructura relacional de todos los nodos del menú.
//********************************************************************************************************************
void Inicializa_Estructura_Menu(void)
{
    if(!Estructura_Menu_Inicializada)
    {
        Ini_Item_Menu(&MenuOp, NULL, &Configuracion, NULL, NULL, (char*)"MENU OPERARIO", NULL);

            Ini_Item_Menu(&Configuracion, &MenuOp, &Muestras, NULL, &EstadoSistema, (char*)"CONFIGURACION", NULL);
                Ini_Item_Menu(&Muestras, &Configuracion, &Intervalo, NULL, &Modem, (char*)"MUESTRAS", NULL);
                    Ini_Item_Menu(&Intervalo, &Muestras, &Intervalo_f, NULL, &Envio, (char*)"INTERVALO", &Eje_Interv_Min_Muestras);
                    Ini_Item_Menu(&Envio, &Muestras, NULL, &Intervalo, NULL, (char*)"PAQUETE ENVIO", &Eje_Interv_Envio_Muestras);
                Ini_Item_Menu(&Modem, &Configuracion, &SmsAlarma, &Muestras, NULL, (char*)"MODEM", NULL);
                    Ini_Item_Menu(&SmsAlarma, &Modem, &EstadoAlarma, NULL, &PinSIM, (char*)"SMS ALARMA", NULL);
                        Ini_Item_Menu(&EstadoAlarma, &SmsAlarma, NULL, NULL, &TlfSmsAlarma, (char*)"ESTADO", &Eje_Config_Envio_SMS_Alarma);
                        Ini_Item_Menu(&TlfSmsAlarma, &SmsAlarma, NULL, &EstadoAlarma, NULL, (char*)"TLF.ENVIO", &Eje_Config_Tlf_Envio_SMS_Alarma);
                    Ini_Item_Menu(&PinSIM, &Modem, NULL, &SmsAlarma, NULL, (char*)"PIN SIM", &Eje_Config_PIN_SIM);

            Ini_Item_Menu(&EstadoSistema, &MenuOp, NULL, &Configuracion, &Memoria, (char*)"ESTADO SISTEMA", &Eje_Estado_Sistema);

            Ini_Item_Menu(&Memoria, &MenuOp, &ResetFabrica, &EstadoSistema, NULL, (char*)"MEMORIA", NULL);
                Ini_Item_Menu(&ResetFabrica, &Memoria, NULL, NULL, &EnvioUART, (char*)"RESET FABRICA", &Eje_Reset_Fabrica);
                Ini_Item_Menu(&EnvioUART, &Memoria, NULL, &ResetFabrica, NULL, (char*)"ENVIO UART", &Eje_Envio_Mem_UART);

        NodoActual = &Configuracion;
    }

    Estructura_Menu_Inicializada = TRUE;
}

//********************************************************************************************************************
//Función que obtiene el valor actual del intervalo programadoo de despiertes para realizar una muestra.
//********************************************************************************************************************
WORD Read_Despiertes_Muestra()
{
    WORD_VAL Resultado;
    BYTE Reintentos = 0;

     do
    {
        Resultado.byte.LB = EEPROM_ReadByte(DIR_DESPIERTES_POR_MEDIDA);
        Resultado.byte.HB = EEPROM_ReadByte(DIR_DESPIERTES_POR_MEDIDA+1);
        Reintentos++;
    }while((Resultado.Val==0xFFFF)&&(Reintentos<3));

    //Si el Resultado es 0xFFFF, significa que la memoria fué inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(Resultado.Val == 0xFFFF)
    {
        Resultado.Val = 0;
    }

    return Resultado.Val;
}

//********************************************************************************************************************
//Función que obtiene el valor actual del número de muestras programado por el que se efectuará un envío por modem.
//********************************************************************************************************************
WORD Read_NumMuestras_Envio()
{
    WORD_VAL Resultado;
    BYTE Reintentos = 0;

     do
    {
        Resultado.byte.LB = EEPROM_ReadByte(DIR_MUESTRAS_POR_ENVIO_MODEM);
        Resultado.byte.HB = EEPROM_ReadByte(DIR_MUESTRAS_POR_ENVIO_MODEM+1);
        Reintentos++;
    }while((Resultado.Val==0xFFFF)&&(Reintentos<3));

    //Si el Resultado es 0xFFFF, significa que la memoria fué inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(Resultado.Val == 0xFFFF)
    {
        Resultado.Val = 0;
    }

    return Resultado.Val;
}

//********************************************************************************************************************
//Función que obtiene la configuración actual de envío de SMS de alarma (Activado o desactivado).
//********************************************************************************************************************
BYTE Read_Estado_Envio_SMS_Alarma()
{
    BYTE Resultado = 0xFF;
    BYTE Reintentos = 0;

    do
    {
        Resultado = EEPROM_ReadByte(DIR_ALARMA_SMS);
        Reintentos++;
    }while((Resultado==0xFF)&&(Reintentos<3));

    //Si el Resultado es 0xFF, significa que la memoria fué inicializada y por tanto el valor del contador lo asignaremos a 0.
    if(Resultado == 0xFF)
    {
        Resultado = ENVIOS_SMS_DESACTIVADO;
    }

    return Resultado;
}

//********************************************************************************************************************
//Función que realiza la lectura del número de teléfono móvil al que serán enviados los mensajes de alarma.
//********************************************************************************************************************
BOOL Read_Tlf_Envio_Alarma(unsigned char *Numero)
{
    BYTE Cifra = 0;
    BYTE Reintentos = 0;
    BOOL Resultado = TRUE;
    BYTE cnt = 0;

    //El teléfono se compone de 9 cifras en ASCII
    for(cnt=0;cnt<=8;cnt++)
    {
        //Hasta que no haya una lectura incorrecta continuamos leyendo
        if(Resultado)
        {
            Cifra=0xFF;
            Reintentos = 0;
            do
            {
                Cifra = EEPROM_ReadByte(DIR_TLF_ALARMA + cnt);
                Reintentos++;
            }while((Cifra==0xFF)&&(Reintentos<3));
            //Si la lectura de la cifra fué correcta se guarda en el array
            if(Reintentos < 3)
            {
                *Numero=Cifra;
                Numero++;
            }
            else{Resultado = FALSE;}
        }
    }
    //Final de línea
    *Numero='\0';
    return Resultado;
}

//********************************************************************************************************************
//Función que realiza la lectura del número PIN.
//********************************************************************************************************************
BOOL Read_PIN_Modem(unsigned char *Numero)
{
    BYTE Cifra = 0;
    BYTE Reintentos = 0;
    BOOL Resultado = TRUE;
    BYTE cnt = 0;

    //El PIN se compone de 4 cifras en ASCII
    for(cnt=0;cnt<=3;cnt++)
    {
        //Hasta que no haya una lectura incorrecta continuamos leyendo
        if(Resultado)
        {
            Cifra=0xFF;
            Reintentos = 0;
            do
            {
                Cifra = EEPROM_ReadByte(DIR_PIN_SIM_MODEM + cnt);
                Reintentos++;
            }while((Cifra==0xFF)&&(Reintentos<3));
            //Si la lectura de la cifra fué correcta se guarda en el array
            if(Reintentos < 3)
            {
                *Numero=Cifra;
                Numero++;
            }
            else{Resultado = FALSE;}
        }
    }
    //Final de línea
    *Numero='\0';
    return Resultado;
}

//********************************************************************************************************************
//Función que realiza la lectura del estado actual del sistema. NORMAL o RECUPERACION BATERIA.
//********************************************************************************************************************
BOOL Read_Estado_Sistema(unsigned char *Estado)
{
    BYTE Reintentos = 0;
    BOOL Resultado = TRUE;

    *Estado = 0xFF;

    Reintentos = 0;
    do
    {
        *Estado = EEPROM_ReadByte(DIR_ESTADO_SISTEMA);
        Reintentos++;
    }while((*Estado==0xFF)&&(Reintentos<3));

    //Comprobación de que la lectura fue correcta
    if(Reintentos >= 3)
    {
        Resultado = FALSE;
    }
    return Resultado;
}

//********************************************************************************************************************
//Función que inicializa la memoria EEPROM a parámetros de fábrica.
//********************************************************************************************************************
BOOL Aplicar_Configuracion_Fabrica()
{
    WORD_VAL Dato;

    //Se inicializa la memoria por completo a 0xFF
    if(!Borrado_Completo_EEPROM()){return FALSE;}

    //Programación de la zona de configuración
    //Despiertes de micro, entre muestras tomadas -> Cada minuto: (15 despiertes)*(4 seg.cada despierte) = 60 segundos.
    Dato.Val=15;
    EEPROM_WriteByte(Dato.byte.LB, DIR_DESPIERTES_POR_MEDIDA);
    EEPROM_WriteByte(Dato.byte.HB, DIR_DESPIERTES_POR_MEDIDA+1);

    //Cantidad de muestras que deben haberse tomado, para realizar el envío via modem (Cada 1 muestra)
    Dato.Val=1;
    EEPROM_WriteByte(Dato.byte.LB, DIR_MUESTRAS_POR_ENVIO_MODEM);
    EEPROM_WriteByte(Dato.byte.HB, DIR_MUESTRAS_POR_ENVIO_MODEM+1);

    //Estado del sistema en modo normal con batería a nivel adecuado
    Dato.Val=ESTADO_NORMAL;
    EEPROM_WriteByte(Dato.byte.LB, DIR_ESTADO_SISTEMA);

    //Envío de SMS de Alarma activado
    Dato.Val=ENVIOS_SMS_ACTIVADO;
    EEPROM_WriteByte(Dato.byte.LB, DIR_ALARMA_SMS);

    //Número de teléfono móvil de envío de SMS de alarma : 630448675
    EEPROM_WriteByte(0x36, DIR_TLF_ALARMA);
    EEPROM_WriteByte(0x33, DIR_TLF_ALARMA + 1);
    EEPROM_WriteByte(0x30, DIR_TLF_ALARMA + 2);
    EEPROM_WriteByte(0x34, DIR_TLF_ALARMA + 3);
    EEPROM_WriteByte(0x34, DIR_TLF_ALARMA + 4);
    EEPROM_WriteByte(0x38, DIR_TLF_ALARMA + 5);
    EEPROM_WriteByte(0x36, DIR_TLF_ALARMA + 6);
    EEPROM_WriteByte(0x37, DIR_TLF_ALARMA + 7);
    EEPROM_WriteByte(0x35, DIR_TLF_ALARMA + 8);

    //Número de PIN de la tarjeta SIM del modem
    EEPROM_WriteByte(0x38, DIR_PIN_SIM_MODEM);
    EEPROM_WriteByte(0x31, DIR_PIN_SIM_MODEM + 1);
    EEPROM_WriteByte(0x38, DIR_PIN_SIM_MODEM + 2);
    EEPROM_WriteByte(0x39, DIR_PIN_SIM_MODEM + 3);

    return TRUE;
}


//////////////////////////
//FUNCIONES HOJA DE ARBOL
//////////////////////////

//********************************************************************************************************************
//Función que controla la gestión de la programación mediante switches del intervalo en minutos entre la toma de muestras.
//********************************************************************************************************************
void Eje_Interv_Min_Muestras(void)
{
    BOOL Salir = FALSE;                 //Indica cuando se saldrá del menú
    BYTE Reintentos = 0;
    static BYTE Min_Interv = 0;
    char StrValor[16];
    WORD_VAL Despiertes;                 //Número de despiertes euqivalentes a programar

    //Se realiza una lectura sobre la memoria para mostrar el valor actual solamente la primera vez,
    //luego el caracter static de la variable almacenará el último valor
    if (Min_Interv == 0)
    {
        //Min_Interv = Read_CNT_Despiertes();
        Despiertes.Val = Read_Despiertes_Muestra();
        Min_Interv = (BYTE)(Despiertes.Val/15);  //Son 15 despiertes por minuto
    }

    //Bucle de selección mediante switches
    do
    {
       //Mostramos en el display el valor actual de minutos de intérvalo de muestra
       sprintf(StrValor,"MINUTOS: %d", Min_Interv);
       LCD_WriteLinea(LCD_LINEA2, (unsigned char*)StrValor );

       Actualiza_Estado_Switches();

       //Incrementar valor en 1
       if(Switch_ON(SW3))
       {
           while(Switch_ON(SW3)) {Actualiza_Estado_Switches();}
           Min_Interv++;


       }
       //Decrementar valor en 1
       else if(Switch_ON(SW6))
       {
           while(Switch_ON(SW6)) {Actualiza_Estado_Switches();}
           if(Min_Interv>1){Min_Interv--;}
       }
       //OK Programar valor en memoria
       else if(Switch_ON(SW5))
       {
            while(Switch_ON(SW5)) {Actualiza_Estado_Switches();}
            
            //Determinamos cuantos despiertes equivales a los minutos de tiempo seleccionado por el usuario
            //El Watchdog = 4 seg. -> 1 min. = 15 despiertes. Por tanto: NºDesp = Minutos selec. * 15
            Despiertes.Val = (WORD)15*Min_Interv;
            
            Reintentos=0;
            do
            {
                EEPROM_WriteByte(Despiertes.byte.LB, DIR_DESPIERTES_POR_MEDIDA);
                EEPROM_WriteByte(Despiertes.byte.HB, DIR_DESPIERTES_POR_MEDIDA+1);
                Reintentos++;
            }
            while(((EEPROM_ReadByte(DIR_DESPIERTES_POR_MEDIDA)!=Despiertes.byte.LB)||(EEPROM_ReadByte(DIR_DESPIERTES_POR_MEDIDA+1)!=Despiertes.byte.HB))&&(Reintentos<3));

            //Indicación del resultado
            if(Reintentos==3)
            {
                LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Error!" );
            }
            else
            {
                LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Guardado!" );
            }
            Inicia_Temporizacion();
            Retardo(2000);
            Salir = TRUE;

       }
       //ESC
       else if(Switch_ON(SW4))
       {
           while(Switch_ON(SW4)) {Actualiza_Estado_Switches();}
           Min_Interv = 0;   //Como no hemos aceptado la cantidad seleccionada, borramos el valor para evitar desincronizaciones entre la memoria y la variable static
           Salir = TRUE;
       }
    }
    while (!Salir);
}

//********************************************************************************************************************
//Función que gestiona la configuración del momento de envío del conjunto de muestras guardadas vía modem.
//********************************************************************************************************************
void Eje_Interv_Envio_Muestras(void)
{
    BOOL Salir = FALSE;                 //Indica cuando se saldrá del menú
    BYTE Reintentos = 0;
    static WORD_VAL Interv_Envio;       //Recoge el número de muestras a acumular antes de efectuar un envío modem.
    char StrValor[16];

    //Se realiza una lectura sobre la memoria para mostrar el valor actual solamente la primera vez,
    //luego el caracter static de la variable almacenará el último valor
    if (Interv_Envio.Val == 0)
    {
        Interv_Envio.Val = Read_NumMuestras_Envio();
    }

    //Bucle de selección mediante switches
    do
    {
       //Mostramos en el display el valor actual de minutos de intérvalo de muestra
       sprintf(StrValor,"CANTIDAD: %d", Interv_Envio.Val);
       LCD_WriteLinea(LCD_LINEA2, (unsigned char*)StrValor );

       Actualiza_Estado_Switches();

       //Incrementar valor en 1
       if(Switch_ON(SW3))
       {
           while(Switch_ON(SW3)) {Actualiza_Estado_Switches();}
           Interv_Envio.Val++;


       }
       //Decrementar valor en 1
       else if(Switch_ON(SW6))
       {
           while(Switch_ON(SW6)) {Actualiza_Estado_Switches();}
           if(Interv_Envio.Val>1){Interv_Envio.Val--;}
       }
       //OK Programar valor en memoria
       else if(Switch_ON(SW5))
       {
            while(Switch_ON(SW5)) {Actualiza_Estado_Switches();}

            Reintentos=0;
            do
            {
                EEPROM_WriteByte(Interv_Envio.byte.LB, DIR_MUESTRAS_POR_ENVIO_MODEM);
                EEPROM_WriteByte(Interv_Envio.byte.HB, DIR_MUESTRAS_POR_ENVIO_MODEM+1);
                Reintentos++;
            }
            while(((EEPROM_ReadByte(DIR_MUESTRAS_POR_ENVIO_MODEM)!=Interv_Envio.byte.LB)||(EEPROM_ReadByte(DIR_MUESTRAS_POR_ENVIO_MODEM+1)!=Interv_Envio.byte.HB))&&(Reintentos<3));

            //Indicación del resultado
            if(Reintentos==3)
            {
                LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Error!" );
            }
            else
            {
                LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Guardado!" );
            }
            Inicia_Temporizacion();
            Retardo(2000);
            Salir = TRUE;

       }
       //ESC
       else if(Switch_ON(SW4))
       {
           while(Switch_ON(SW4)) {Actualiza_Estado_Switches();}
           Interv_Envio.Val = 0;  //Como no hemos aceptado la cantidad seleccionada, borramos el valor para evitar desincronizaciones entre la memoria y la variable static
           Salir = TRUE;
       }
    }
    while (!Salir);
}

//********************************************************************************************************************
//Función que programa si el envío de SMS de alarma está activado o desactivado.
//********************************************************************************************************************
void Eje_Config_Envio_SMS_Alarma(void)
{
    BOOL Salir = FALSE;                   //Indica cuando se saldrá del menú
    BYTE Reintentos = 0;
    static BYTE Estado_Activacion = 0xFF; //Indica el estado de la opción de envío de SMS. Inicializado a un valor sin interpretación.
    char StrValor[16];

    //Se realiza una lectura sobre la memoria para mostrar el valor actual solamente la primera vez,
    //luego el caracter static de la variable almacenará el último valor
    if (Estado_Activacion == 0xFF)
    {
        Estado_Activacion = Read_Estado_Envio_SMS_Alarma();
    }

    //Bucle de selección mediante switches
    do
    {
       //Mostramos en el display el valor actual de minutos de intérvalo de muestra
       if(Estado_Activacion == ENVIOS_SMS_ACTIVADO){ sprintf(StrValor,"<ACTIVADO>"); }
       if(Estado_Activacion == ENVIOS_SMS_DESACTIVADO){ sprintf(StrValor,"<DESACTIVADO>"); }
       LCD_WriteLinea(LCD_LINEA2, (unsigned char*)StrValor );

       Actualiza_Estado_Switches();

       //Alternamos el valor de activado a desactivado (Toggle)
       if(Switch_ON(SW3))
       {
           while(Switch_ON(SW3)) {Actualiza_Estado_Switches();}
           if(Estado_Activacion == ENVIOS_SMS_ACTIVADO){Estado_Activacion = ENVIOS_SMS_DESACTIVADO;}
           else{Estado_Activacion = ENVIOS_SMS_ACTIVADO;}
       }
       //Alternamos el valor de activado a desactivado (Toggle)
       else if(Switch_ON(SW6))
       {
           while(Switch_ON(SW6)) {Actualiza_Estado_Switches();}
           if(Estado_Activacion == ENVIOS_SMS_ACTIVADO){Estado_Activacion = ENVIOS_SMS_DESACTIVADO;}
           else{Estado_Activacion = ENVIOS_SMS_ACTIVADO;}
       }
       //OK Programar valor en memoria
       else if(Switch_ON(SW5))
       {
            while(Switch_ON(SW5)) {Actualiza_Estado_Switches();}

            Reintentos=0;
            do
            {
                EEPROM_WriteByte(Estado_Activacion, DIR_ALARMA_SMS);
                Reintentos++;
            }
            while((EEPROM_ReadByte(DIR_ALARMA_SMS)!=Estado_Activacion)&&(Reintentos<3));

            //Indicación del resultado
            if(Reintentos==3)
            {
                LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Error!" );
            }
            else
            {
                LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Guardado!" );
            }
            Inicia_Temporizacion();
            Retardo(2000);
            Salir = TRUE;
       }
       //ESC
       else if(Switch_ON(SW4))
       {
           while(Switch_ON(SW4)) {Actualiza_Estado_Switches();}
           Estado_Activacion = 0xFF;  //Como no hemos aceptado la cantidad seleccionada, borramos el valor para evitar desincronizaciones entre la memoria y la variable static
           Salir = TRUE;
       }
    }
    while (!Salir);
}

//********************************************************************************************************************
//Función que permite programar el número de teléfono al que enviar los SMS de alarmas.
//********************************************************************************************************************
void Eje_Config_Tlf_Envio_SMS_Alarma(void)
{
    BYTE NumTlf[10] = {'0','0','0','0','0','0','0','0','0','\0'}; //Dígitos en ASCII del número de teléfono. La cadena termina con el caracter nulo '\0'
    BOOL Salir = FALSE;                //Indica cuando se saldrá del menú
    BYTE NumCifraSelec = 0;            //Recoge la cifra actualmente seleccionada para modificar
    BYTE Estado = SELECCIONANDO_CIFRA; //Recoge el estado actual de operación: Seleccionando o modificando cifra.
    WORD CntPulsacionLarga = 0;        //Contador para determinar si una pulsación fué larga
    BYTE Info[3] = {'S','0','\0'};     //Cadena que recoge la información del modo seleccionado y la posición del cursor en el display
    BYTE Reintentos = 0;
    BYTE idCifra = 0;

    //Lectura del teléfono actualmente programado
    if(Read_Tlf_Envio_Alarma(NumTlf))
    {
        LCD_WriteLinea(LCD_LINEA2, NumTlf);
    }
    else
    {
        LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"000000000");
        *NumTlf = (unsigned char)"000000000";
    }

    //Bucle de selección mediante switches
    do
    {
       //Mostramos en el display el telefono actualmente seleccionado
       if (Estado == SELECCIONANDO_CIFRA){Info[0] = 'S';}
       if (Estado == MODIFICANDO_CIFRA){Info[0] = 'M';}
       Info[1] = '1' + NumCifraSelec;
       LCD_WrString_LinPos(LCD_LINEA1, 13, Info);
       LCD_WriteLinea(LCD_LINEA2, NumTlf );

       //Mostramos en el display las indicación sobre la cifra actualmente en selección o modificación
       //LCD_WrString_LinPos(LCD_LINEA2, 10, Estado);
       //LCD_WrString_LinPos(LCD_LINEA2, 12, (unsigned char)('0' + NumCifraSelec));
       
       Actualiza_Estado_Switches();

       //Seleccionar cifra izquierda o decrementar cifra
       if(Switch_ON(SW3))
       {
           while(Switch_ON(SW3)) {Actualiza_Estado_Switches();}
           //En modo selección: Seleccion de la siguiente cifra a la izquierda
           if(Estado == SELECCIONANDO_CIFRA)
           {
               //Si no estamos en la cifra más a la izquierda, desplazamos
               if(NumCifraSelec > ID_PRIMERA_CIFRA)
               {
                   NumCifraSelec--;
               }
           }
           //En modo modificación: Decrementamos el valor de la cifra actualmente seleccionada
           if(Estado == MODIFICANDO_CIFRA)
           {
               if(NumTlf[NumCifraSelec] >= '1')
               {
                   NumTlf[NumCifraSelec] -= 1;
               }
           }
       }
       //Seleccionar cifra derecha o incrementar cifra
       else if(Switch_ON(SW6))
       {
           while(Switch_ON(SW6)) {Actualiza_Estado_Switches();}
           //En modo selección: Seleccion de la siguiente cifra a la derecha
           if(Estado == SELECCIONANDO_CIFRA)
           {
              //Si no estamos en la cifra más a la derecha, desplazamos
               if(NumCifraSelec < ID_ULTIMA_CIFRA)
               {
                   NumCifraSelec++;
               }
           }
           //En modo modificación: Incrementamos el valor de la cifra actualmente seleccionada
           if(Estado == MODIFICANDO_CIFRA)
           {
               if(NumTlf[NumCifraSelec] < '9')
               {
                   NumTlf[NumCifraSelec] += 1;
               }
           }
       }
       //OK Conmutar entre seleccionar cifra, modificar cifra y programar el teléfono en memoria
       else if(Switch_ON(SW5))
       {
           CntPulsacionLarga = 0;  //Reset del contador
           while((Switch_ON(SW5))&&(CntPulsacionLarga < PULSACION_LARGA))
           {
               Actualiza_Estado_Switches();
               CntPulsacionLarga++;
           }

           //Si se detecta pulsación larga, se programa el teléfono en memoria
           if(CntPulsacionLarga == PULSACION_LARGA)
           {
               for(idCifra = ID_PRIMERA_CIFRA; idCifra <= ID_ULTIMA_CIFRA; idCifra++)
               {
                    Reintentos=0;
                    do
                    {
                        EEPROM_WriteByte(NumTlf[idCifra], DIR_TLF_ALARMA + idCifra);
                        Reintentos++;
                    }
                    while((EEPROM_ReadByte(DIR_TLF_ALARMA + idCifra)!= NumTlf[idCifra])&&(Reintentos<3));

                    //Indicación del resultado
                    if(Reintentos==3){ break; }
               }
               
               if (Reintentos == 3)
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Error!" );
               }
               else
               {
                    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Guardado!" );
               }
               Inicia_Temporizacion();
               Retardo(2000);
               Salir = TRUE;
           }
           else
           {
               //En modo selección: Cambio a modo modificación de cifra
               if(Estado == SELECCIONANDO_CIFRA)
               {
                   Estado = MODIFICANDO_CIFRA;
               }
               //En modo modificación: Cambio a modo de selección de cifra
               else if(Estado == MODIFICANDO_CIFRA)
               {
                   Estado = SELECCIONANDO_CIFRA;
               }
           }
       }
       //ESC
       else if(Switch_ON(SW4))
       {
           while(Switch_ON(SW4)) {Actualiza_Estado_Switches();}

           //En modo selección: Salir de la opción finalmente
           if(Estado == SELECCIONANDO_CIFRA)
           {
               Salir = TRUE;
           }
           //En modo modificación: Cambio a modo de selección de cifra
           if(Estado == MODIFICANDO_CIFRA)
           {
               Estado = SELECCIONANDO_CIFRA;
           }
       }
    }while (!Salir);
}

//********************************************************************************************************************
//Función que permite programar el PIN para la activación de la tarjeta SIM del modem de comunicaciones.
//********************************************************************************************************************
void Eje_Config_PIN_SIM(void)
{
    BYTE NumPIN[5] = {'0','0','0','0','\0'}; //Dígitos en ASCII del PIN. La cadena termina con el caracter nulo '\0'
    BOOL Salir = FALSE;                //Indica cuando se saldrá del menú
    BYTE NumCifraSelec = 0;            //Recoge la cifra actualmente seleccionada para modificar
    BYTE Estado = SELECCIONANDO_CIFRA; //Recoge el estado actual de operación: Seleccionando o modificando cifra.
    WORD CntPulsacionLarga = 0;        //Contador para determinar si una pulsación fué larga
    BYTE Info[3] = {'S','0','\0'};     //Cadena que recoge la información del modo seleccionado y la posición del cursor en el display
    BYTE Reintentos = 0;
    BYTE idCifra = 0;

    //Lectura del PIN actualmente programado
    if(Read_PIN_Modem(NumPIN))
    {
        LCD_WriteLinea(LCD_LINEA2, NumPIN);
    }
    else
    {
        LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"0000");
        *NumPIN = (unsigned char)"0000";
    }

    //Bucle de selección mediante switches
    do
    {
       //Mostramos en el display el PIN actualmente seleccionado
       if (Estado == SELECCIONANDO_CIFRA){Info[0] = 'S';}
       if (Estado == MODIFICANDO_CIFRA){Info[0] = 'M';}
       Info[1] = '1' + NumCifraSelec;
       LCD_WrString_LinPos(LCD_LINEA1, 13, Info);
       LCD_WriteLinea(LCD_LINEA2, NumPIN );


       Actualiza_Estado_Switches();

       //Seleccionar cifra izquierda o decrementar cifra
       if(Switch_ON(SW3))
       {
           while(Switch_ON(SW3)) {Actualiza_Estado_Switches();}
           //En modo selección: Seleccion de la siguiente cifra a la izquierda
           if(Estado == SELECCIONANDO_CIFRA)
           {
               //Si no estamos en la cifra más a la izquierda, desplazamos
               if(NumCifraSelec > ID_PRIMERA_CIFRA)
               {
                   NumCifraSelec--;
               }
           }
           //En modo modificación: Decrementamos el valor de la cifra actualmente seleccionada
           if(Estado == MODIFICANDO_CIFRA)
           {
               if(NumPIN[NumCifraSelec] >= '1')
               {
                   NumPIN[NumCifraSelec] -= 1;
               }
           }
       }
       //Seleccionar cifra derecha o incrementar cifra
       else if(Switch_ON(SW6))
       {
           while(Switch_ON(SW6)) {Actualiza_Estado_Switches();}
           //En modo selección: Seleccion de la siguiente cifra a la derecha
           if(Estado == SELECCIONANDO_CIFRA)
           {
              //Si no estamos en la cifra más a la derecha, desplazamos
               if(NumCifraSelec < ID_ULTIMA_CIFRA)
               {
                   NumCifraSelec++;
               }
           }
           //En modo modificación: Incrementamos el valor de la cifra actualmente seleccionada
           if(Estado == MODIFICANDO_CIFRA)
           {
               if(NumPIN[NumCifraSelec] < '9')
               {
                   NumPIN[NumCifraSelec] += 1;
               }
           }
       }
       //OK Conmutar entre seleccionar cifra, modificar cifra y programar el teléfono en memoria
       else if(Switch_ON(SW5))
       {
           CntPulsacionLarga = 0;  //Reset del contador
           while((Switch_ON(SW5))&&(CntPulsacionLarga < PULSACION_LARGA))
           {
               Actualiza_Estado_Switches();
               CntPulsacionLarga++;
           }

           //Si se detecta pulsación larga, se programa el teléfono en memoria
           if(CntPulsacionLarga == PULSACION_LARGA)
           {
               for(idCifra = ID_PRIMERA_CIFRA_PIN; idCifra <= ID_ULTIMA_CIFRA_PIN; idCifra++)
               {
                    Reintentos=0;
                    do
                    {
                        EEPROM_WriteByte(NumPIN[idCifra], DIR_PIN_SIM_MODEM + idCifra);
                        Reintentos++;
                    }
                    while((EEPROM_ReadByte(DIR_PIN_SIM_MODEM + idCifra)!= NumPIN[idCifra])&&(Reintentos<3));

                    //Indicación del resultado
                    if(Reintentos==3){ break; }
               }

               if (Reintentos == 3)
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Error!" );
               }
               else
               {
                    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Guardado!" );
               }
               Inicia_Temporizacion();
               Retardo(2000);
               Salir = TRUE;
           }
           else
           {
               //En modo selección: Cambio a modo modificación de cifra
               if(Estado == SELECCIONANDO_CIFRA)
               {
                   Estado = MODIFICANDO_CIFRA;
               }
               //En modo modificación: Cambio a modo de selección de cifra
               else if(Estado == MODIFICANDO_CIFRA)
               {
                   Estado = SELECCIONANDO_CIFRA;
               }
           }
       }
       //ESC
       else if(Switch_ON(SW4))
       {
           while(Switch_ON(SW4)) {Actualiza_Estado_Switches();}

           //En modo selección: Salir de la opción finalmente
           if(Estado == SELECCIONANDO_CIFRA)
           {
               Salir = TRUE;
           }
           //En modo modificación: Cambio a modo de selección de cifra
           if(Estado == MODIFICANDO_CIFRA)
           {
               Estado = SELECCIONANDO_CIFRA;
           }
       }
    }while (!Salir);
}

//********************************************************************************************************************
//Función que permite consultar el estado actual del sistema. NORMAL o RECUPERACION BATERIA.
//********************************************************************************************************************
void Eje_Estado_Sistema(void)
{
   BYTE Estado_Sistema = 0xFF;    //Indica el estado del sistema
   char StrValor[16];
    
   //Lectura de la posción que recoge el estado del sistema
   Read_Estado_Sistema(&Estado_Sistema);
   
   //Mostramos en el display el estado actual del sistema
   if(Estado_Sistema == ESTADO_NORMAL)
   {
       sprintf(StrValor,"<NORMAL>");
   }
   else if(Estado_Sistema == ESTADO_RECUP_BAT)
   {
       sprintf(StrValor,"<RECUP. BATERIA>");
   }
   else
   {
       sprintf(StrValor,"<DESCONOCIDO>");
   }

   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)StrValor );
   Inicia_Temporizacion();
   Retardo(2000);
    
}

//********************************************************************************************************************
//Función que permite situar los valores de la memoria EEPROM a unos valores iniciales y conocidos como parámetros de fábrica.
//********************************************************************************************************************
void Eje_Reset_Fabrica(void)
{
     LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Ini. EEPROM...  ");
     //Grabación de parámetros fijos iniciales
     if(Aplicar_Configuracion_Fabrica())
     {
         LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"OK!");
     }
     else
     {
         LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Error!");
     }

     Inicia_Temporizacion();
     Retardo(2000);
}

//********************************************************************************************************************
//Función que realiza el envío de la memoria completa EEPROM, byte a byte, por el puerto serie.
//********************************************************************************************************************
void Eje_Envio_Mem_UART(void)
{
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Enviando...  ");
    Envio_Memoria_UART();
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"OK!");
    Inicia_Temporizacion();
    Retardo(2000);
}

//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PUBLICOS
//********************************************************************************************************************
//********************************************************************************************************************

//********************************************************************************************************************
//Función que permite inicializar la estructura en arbol de los nodos que forman el menú operario.
//Contiene el buque principal que permite moverse mediante los switches por los nodos, y configurar los parámetros.
//********************************************************************************************************************
void MenuOperario (void)
{
    //Indica cuando se saldrá del menú
    BOOL Salir = FALSE;

    //Cadenas de apoyo
    char cadena[16];
    char CaracterMain[]=">";

    //Inicialización periféricos que intervienen en el menú
    Inicializacion_Modulo_EEPROM();
    Inicializa_Switches();
    LCD_Inicializacion();
    LCD_Clear();

    //Inicialización de la estructura arbol del menú
    Inicializa_Estructura_Menu();

    //Bucle de estancia en el menú
    do
    {
         Actualiza_Estado_Switches();

       //Izquierda
       if(Switch_ON(SW3))
       {
           while(Switch_ON(SW3)) {Actualiza_Estado_Switches();}
           NodoActual = Move_Item_IZQ(NodoActual);

       }
       //Derecha
       else if(Switch_ON(SW6))
       {
           while(Switch_ON(SW6)) {Actualiza_Estado_Switches();}
           NodoActual = Move_Item_DCH(NodoActual);

       }
       //OK
       else if(Switch_ON(SW5))
       {
           while(Switch_ON(SW5)) {Actualiza_Estado_Switches();}
           if (NodoActual->funcion != NULL)
           {
               NodoActual->funcion();
           }
           else
           {
            NodoActual = Move_Item_DOWN(NodoActual);
           }
       }
       //ESC
       else if(Switch_ON(SW4))
       {
           while(Switch_ON(SW4)) {Actualiza_Estado_Switches();}
           //if(NodoActual->Padre == &Main)
           if(NodoActual->Padre == &MenuOp)
           {
               //NodoActual = &Config_Sys;
               NodoActual = &Configuracion;
               Salir = TRUE;
           }
           else
           {
            //NodoActual = Move_Item_UP(NodoActual,&Main);
            NodoActual = Move_Item_UP(NodoActual,&MenuOp);
           }
       }

       //Actualización del display con la nueva pulsación
       strcpy(cadena, "               ");
       strcpy(cadena, CaracterMain);
       strcat(cadena, (char*)NodoActual->Padre->Texto);
       LCD_WriteLinea(LCD_LINEA1, (unsigned char*)cadena );
       LCD_WriteLinea(LCD_LINEA2, NodoActual->Texto);
    }
    while (!Salir);

    LCD_Clear();
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Saliendo..." );
    Inicia_Temporizacion();
    Retardo(2000);
    LCD_Clear();

}

