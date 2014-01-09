#include <stdio.h>
#include "App_Main.h"
#include "Temporizacion.h"
#include "PMP_LCD.h"
#include "SPI_EEPROM.h"
#include "Gestor_EEPROM.h"
#include "Switches.h"
#include "leds.h"
#include "Gestor_Bateria.h"
#include "string.h"
#include "Gestor_Sensores.h"
#include "Rele.h"
#include "MenuOperario.h"
#include "Reloj_RTCC.h"
#include "Modem.h"


//********************************************************************************************************************    
//********************************************************************************************************************    
//                                                VARIABLES MIEMBRO
//********************************************************************************************************************    
//********************************************************************************************************************    


//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PRIVADOS
//********************************************************************************************************************
//********************************************************************************************************************
void Mensaje_Bienvenida(void);
void Procesa_Evento_Menu(void);
void Procesa_Modo_Recuperacion(void);
void Procesa_Reset_No_Controlado(void);
void Procesa_Evento_UART(void);
void Procesa_Evento_Sensores_Bateria(void);
void Ejecuta_Peticion_Recibida(char * Comando);
void Visualiza_Ultima_Muestra_LCD(SENSORES);

//********************************************************************************************************************
//Función que muestra en el display el mensaje inicial al arrancar el sistema.
//********************************************************************************************************************
void Mensaje_Bienvenida()
{
    LCD_Clear();
    LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"EUPLA-METEO '14 ");
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"      v1.0      ");
    Retardo(2000);
    LCD_Clear();
}

//********************************************************************************************************************
//Función que se encarga de gestionar el movimiento del usuario a través del menú operario.
//********************************************************************************************************************
void Procesa_Evento_Menu()
{
    MenuOperario();
}

//********************************************************************************************************************
//Función que se encarga de poner al sistema en modo de bajo consumo para fomentar la recuperación de la batería.
//********************************************************************************************************************
void Procesa_Modo_Recuperacion()
{
    Inicializacion_Modulo_EEPROM();
    Activar_Modo_Recuperacion();
}

//********************************************************************************************************************
//Función que gestiona el procesamiento de un posible reset por watchdog (fallo en el programa) o 
//por software (excepción y trap).
//********************************************************************************************************************
void Procesa_Reset_No_Controlado()
{
    LCD_Clear();
    LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"    ATENCION!   ");
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)" RESET SISTEMA  ");
    Retardo(2000);
}

//********************************************************************************************************************
//Función que atiende a los eventos llegados a través del puerto de comunicaciónes donde 
//se encuentra instalado el modem GSM/GPRS.
//********************************************************************************************************************
void Procesa_Evento_UART()
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;

   //Inicializamos buffer de datos
   for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}

   //Mensaje LCD: Recibiendo..
   LCD_Clear();
   LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"MODEM HABILITADO");
   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Recibiendo...   ");

   //Esperamos a recibir todos los datos.. Si pasan 2 segundos sin recibir nada,
   //se entiende que la comunicación finalizó y salimos..
   Reset_Contador_MSeg();
   while(Lectura_Contador_MSeg()<2000)
   {
       if(UART2_DatosPorLeer())
       {
           Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
           for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
           UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
           strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer
       }
   }

   //Procesado del comando recibido..
   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Procesando...   ");
   Ejecuta_Peticion_Recibida((char *)DatosRecibidos_UART2);
   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fin Proceso...  ");
   Retardo(1000);
}

//********************************************************************************************************************
//Función que realiza las acciones o comandos llegados vía modem.
//********************************************************************************************************************
void Ejecuta_Peticion_Recibida(char * Comando)
{
    LCD_Clear();
    LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"  Procesando!   ");
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)Comando);
    Retardo(2000);
}

//********************************************************************************************************************
//Función que gestiona las medidas sobre los sensores y el control sobre el nivel de la batería.
//Se comprueba si el nivel de la batería está crítico, para pasar a modo de recuperación.
//En caso de no estar en modo recuperación, se actualizan los contadores de número de despiertes
//para determinar si ha llegado el momento de realizar un muestreo de los sensores o incluso un
//envío de datos vía modem.
//********************************************************************************************************************
void Procesa_Evento_Sensores_Bateria()
{
    SENSORES Muestra;
    WORD Dir_Sig_E2P_Libre = 0;
    BOOL EEPROM_Llena = FALSE;
    BYTE cnt = 0;
    BYTE Info[3] = {'\0','\0','\0'}; //Cuenta regresiva
    CALIDAD_COBERTURA CalidadCobertura;
    BYTE Reintentos = 0;
    BOOL RegistroPIN = FALSE;

    //Preparación de la comunicación con la memoria
    Inicializacion_Modulo_EEPROM();

    //Se revisa el nivel de la batería
    NIVEL_BATERIA Nivel = Comprobacion_Estado_Bateria();

    //Si la batería está a nivel mínimo, entramos en modo recuperación
    if(Nivel == NIVEL_BAJO)
    {
       Activar_Modo_Recuperacion();
       LEDs_ON;
       LCD_Clear();
       LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"Batería Baja!   ");
       LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Recuperacion ON ");
       Retardo(1500);
    }

    //Si el sistema está en modo recuperación de la batería, comprobamos si se vuelve a modo normal de trabajo.
    if(Modo_Recuperacion())
    {
        if(Nivel < NIVEL_MEDIO_ALTO)
        {
           LEDs_ON;
           LCD_Clear();
           LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"Recuperando...  ");
           LCD_WriteLinea(LCD_LINEA2, Porcentaje_Nivel_Bateria(Nivel));
           Retardo(1500);
        }
        else
        {
           Desactivar_Modo_Recuperacion();
           LCD_Clear();
           LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"Recuperado..!   ");
           LCD_WriteLinea(LCD_LINEA2, Porcentaje_Nivel_Bateria(Nivel));
           Retardo(1500);
        }
    }
    else
    {
        //Incrementamos el contador de despiertes
        Inc_CNT_Despiertes();

        //Como no estamos en modo recuperación, comprobamos si corresponde realizar un scan sobre los sensores.
        if(Si_Realizar_Medidas_Sensores())
        {
           //Lectura y almacenamiento de valores de los sensores
           LCD_Clear();
           LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"Muestreando     ");
           LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Sensores ON..   ");
           Retardo(1500);

           //Contador de despiertes inicializado a cero.
           Reset_CNT_Despiertes();

           //Inicialización de la variable
           Muestra.Nivel_Bateria=0xFFFF;
           Muestra.Pluviometria=0xFFFF;
           Muestra.Temperatura=0xFFFF;
           Muestra.Vel_Aire=0xFFFF;

           //Muestreo de sensores
           Muestra = Barrido_Sensores();

           //Comprobación de que la muestra se tomó correctamente.
           if((Muestra.Nivel_Bateria == 0xFFFF)||(Muestra.Pluviometria == 0xFFFF)||(Muestra.Temperatura == 0xFFFF)||(Muestra.Vel_Aire == 0xFFFF))
           {
               LCD_Clear();
               LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"Error           ");
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"de Muestreo..!  ");
               Retardo(1500);
               return;
           }

           //Obtenemos la siguiente dirección libre para ubicar la muestra en EEPROM
           Dir_Sig_E2P_Libre = Siguiente_Direccion_Libre_EEPROM();

           //Comprobación del rebase de la memoria. En caso de superarlo se ajusta a DIR_BASE_MEDIDAS.
           if(Dir_Sig_E2P_Libre < (TAM_MEMORIA_EEPROM - BYTES_POR_MEDIDA))
           {
               //Guardado de la muestra en memoria
               if(Guarda_Muestra_EEPROM(Muestra, Dir_Sig_E2P_Libre))
               {
                  //Incrementamos el número de muestras tomadas
                  Inc_CNT_Muestras_Tomadas();
               }

               //Presentación en display de la última muestra tomada
               Visualiza_Ultima_Muestra_LCD(Muestra);
           }
           //Si la memoria está llena de medidas, forzamos el envío al servidor.
           else
           {
               EEPROM_Llena = TRUE;
           }

           //Comprobación si corresponde enviar las muestras via modem.
           if((EEPROM_Llena)||(Si_Realizar_Envio_Muestras_Modem()))
           {
               Cierra_Com_Modem(); //Para no recibir la información enviada al alimentar el modem.
               MODEM_ON;

               //Cuenta regresiva para inicialización del modem tras alimentación
               LCD_Clear();
               LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"INICIANDO MODEM ");
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Espere..  10 seg");
               Retardo(1000);
               for(cnt=9; cnt!=0; cnt--)
               {
                   Info[0] = ' ';
                   Info[1] = 0x30 + cnt;
                   LCD_WrString_LinPos(LCD_LINEA2,10,Info);
                   Retardo(1000);
               }

               //REGISTRO EN LA RED DEL MODEM///////////////////////////////////
               Inicializacion_Com_Modem();
               LCD_Clear();
               LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"REGISTRO RED GSM");
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Buscando modem..");

               //Comprobación si el modem está en línea de comandos
               if(!AT())
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Comunicacion NOK");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Comprobación de la cobertura
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Cobertura:      ");
               CalidadCobertura=AT_CSQ();
               if(CalidadCobertura < INTENSIDAD_BUENA)
               {
                   LCD_WrString_LinPos(LCD_LINEA2, 13, (unsigned char*)"NOK");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               else
               {
                   if(CalidadCobertura == INTENSIDAD_BUENA){LCD_WrString_LinPos(LCD_LINEA2, 11, (unsigned char*)"BUENA"); Retardo(500);}
                   else{LCD_WrString_LinPos(LCD_LINEA2, 11, (unsigned char*)"EXCNT"); Retardo(500);}
               }
               Retardo(1000);
               //Insercción del PIN para red GSM
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Insertando PIN..");
               if(!AT_CREG())
               {
                   AT_CPIN((unsigned char*)"8189");
                   do
                   {
                       RegistroPIN = AT_CREG();
                       Reintentos++;
                   }
                   while((!RegistroPIN) && (Reintentos < 100));
                   if(RegistroPIN)
                   {
                       LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"PIN OK          ");
                       Retardo(500);
                   }
                   else
                   {
                       LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"PIN NOK         ");
                       Retardo(5000);
                       MODEM_OFF;
                       return;
                   }
               }
               Retardo(1000);

               //CONFIGURACION CONEXION INTERNET DEL MODEM///////////////////////////////////
               LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"CONFIG. CONEXION");
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Activar pila TCP");
               //Arranque de la pila TCP/IP del modem
               if(!AT_WOPEN('1'))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuración del modem a modo GPRS
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Activar mod.GPRS");
               if(!AT_GPRSMODE('1'))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuración punto de acceso a internet: Servidor
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Conf. Serv. APN ");
               if(!AT_APNSERV((unsigned char*)"movistar.es"))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuración punto de acceso a internet: Usuario
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Conf.Usuario APN");
               if(!AT_APNUN((unsigned char*)"movistar.es"))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuración punto de acceso a internet: Password
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Conf.Passw. APN ");
               if(!AT_APNPW((unsigned char*)""))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuración enlace TCP: Dirección IP
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"TCP: Dir.IP Serv");
               if(!AT_TCPSERV((unsigned char*)"cessor.no-ip.biz"))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuración enlace TCP: Puerto
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"TCP: Puerto Serv");
               if(!AT_TCPPORT((unsigned char*)"20000"))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuración enlace TCP: Retardo de transmisión
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"TCP: Delay TXT  ");
               if(!AT_TCPTXDELAY((unsigned char*)"100"))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuración enlace TCP: Cierre de socket mediante caracter ETX
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"TCP: OFF ETX    ");
               if(!AT_DLEMODE('1'))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);

               //ESTABLECIMIENTO ENLACE TCP CON SERVIDOR///////////////////////////////////
               LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"ENLACE TCP/IP   ");
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Attach serv.GPRS");
               //Enlace con el servicio GPRS del APN
               if(!AT_CGATT('1'))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }

               //Esperamos a que el modem se conecte al servicio GPRS
               RegistroPIN = FALSE;
               Reintentos = 0;
               do
               {
                   RegistroPIN = AT_CNUM();
                   Reintentos++;
               }
               while((!RegistroPIN) && (Reintentos < 10));

               //Enlace a internet mediante APN
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Asignando IP... ");
               if(!AT_CONNECTIONSTART())
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Apertura de socket con el servidor Splunk
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Abriendo socket ");
               if(!AT_OTCP())
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);

               //Envío de las medidas almacenadas en memoria
               LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"CONX.ESTABLECIDA");
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Enviando Datos..");
               if(Enviar_Muestras_Modem())
               {
                   Reset_CNT_Muestras_Tomadas();
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Datos enviandos!");
                   Retardo(1000);
               }

               //Cierre de socket con el servidor Splunk
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Cerrando socket ");
               if(!Caracter_ETX())
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Desconexión a internet mediante APN
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Internet OFF    ");
               if(!AT_CONNECTIONSTOP())
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Desenlace del servicio GPRS del APN
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Detach serv.GPRS");
               if(!AT_CGATT('0'))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               MODEM_OFF;
           }
        }
    }
}

//********************************************************************************************************************
//Función que realiza la lectura de la última muestra tomada en EEPROM y la visualiza en el display LCD.
//********************************************************************************************************************
void Visualiza_Ultima_Muestra_LCD(SENSORES Muestra)
{
    char Informacion[16];
    unsigned char Info_LCD_LineaUP[16];
    unsigned char Info_LCD_LineaDW[16];
    unsigned char * Puntero;
    BYTE i=0;

    //Inicialización de los array LCD
    for(i=0; i<16; i++)
    {
        Info_LCD_LineaUP[i]= ' ';
        Info_LCD_LineaDW[i]= ' ';
    }

    //Montaje de la información para mostrarlo en el display LCD
    Puntero = Info_LCD_LineaUP;
    //Temperatura
    *Puntero = 'T';
    Puntero++;
    *Puntero = ':';
    Puntero++;
    sprintf(Informacion, "%d", Muestra.Temperatura);
    for(i=0; i<16; i++)
    {
        if(Informacion[i]!='\0')
        {
            *Puntero = (unsigned char)Informacion[i];
            Puntero++;
        }
        else
        {
            *Puntero = *(Puntero-1);
            *(Puntero-1) = '.';
            Puntero++;
            break;
        }
    }
    *Puntero = 'c';
    Puntero++;
    //Ajuste de la cadena para que no aparezcan desplazados en caso de variar el nº de cifras
    //Todos datos son como máximo de 3 cifras.
    for(i=i; i<3; i++)
    {
        *Puntero = ' ';
        Puntero++;
    }

    //Pluviometria
    *Puntero = ' ';
    Puntero++;
    *Puntero = ' ';
    Puntero++;
    *Puntero = 'P';
    Puntero++;
    *Puntero = ':';
    Puntero++;
    sprintf(Informacion, "%d", Muestra.Pluviometria);
    for(i=0; i<16; i++)
    {
        if(Informacion[i]!='\0')
        {
            *Puntero = (unsigned char)Informacion[i];
            Puntero++;
        }
        else{break;}
    }
    *Puntero = 'm';
    Puntero++;
    *Puntero = 'm';
    Puntero++;

    //Vel.Aire
    Puntero = Info_LCD_LineaDW;
    *Puntero = 'V';
    Puntero++;
    *Puntero = ':';
    Puntero++;
    sprintf(Informacion, "%d", Muestra.Vel_Aire);
    for(i=0; i<16; i++)
    {
        if(Informacion[i]!='\0')
        {
            *Puntero = (unsigned char)Informacion[i];
            Puntero++;
        }
        else{break;}
    }
    *Puntero = 'k';
    Puntero++;
    *Puntero = 'm';
    Puntero++;
    *Puntero = 'h';
    Puntero++;
    //Ajuste de la cadena para que no aparezcan desplazados en caso de variar el nº de cifras
    //Todos datos son como máximo de 3 cifras.
    for(i=i; i<3; i++)
    {
        *Puntero = ' ';
        Puntero++;
    }

    //Nivel batería
    *Puntero = ' ';
    Puntero++;
    *Puntero = 'B';
    Puntero++;
    *Puntero = ':';
    Puntero++;
    sprintf(Informacion, "%d", Muestra.Nivel_Bateria);
    for(i=0; i<16; i++)
    {
        if(Informacion[i]!='\0')
        {
            *Puntero = (unsigned char)Informacion[i];
            Puntero++;
        }
        else{break;}
    }
    *Puntero = '%';

    LCD_Clear();
    LCD_WriteLinea(LCD_LINEA1, Info_LCD_LineaUP);
    LCD_WriteLinea(LCD_LINEA2, Info_LCD_LineaDW);
    Retardo(10000);

}
//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PUBLICOS
//********************************************************************************************************************
//********************************************************************************************************************

//********************************************************************************************************************
//Función que realiza la incialización de los diferentes módulos utilizados tras un Reset.
//********************************************************************************************************************
void Inicializa_Sistema(void)
{
    //Inicializaciones de periféricos
    LCD_Inicializacion();
    Inicia_Temporizacion();
    UART2_Configura_Abre();

    //Puesta en hora del reloj RTC
    Rtcc_Inicializacion();        //Habilita el reloj secundario y coloca el reloj en reposo
    rtccFechaHora FechaHoraReloj;
    FechaHoraReloj.w[0] = 0x0014; //Anio
    FechaHoraReloj.w[1] = 0x0109; //Mes - Dia
    FechaHoraReloj.w[2] = 0x0220; //Dia Semana - Hora (0-lunes, 1-martes..)
    FechaHoraReloj.w[3] = 0x3000; //Minutos - Segundos
    Rtcc_Configuracion_FechaHora_Reloj(&FechaHoraReloj);
    Rtcc_Activacion();  //Activación del reloj. Habilita tambien la interrupción

    //Desactivación del modem. La estrategia será que se activará solamente cuando se envíen
    //las medidas al servidor.
    MODEM_OFF;

    //Deshabilitar módulos no utilizados. Eliminar consumo innecesario.
    _SPI1MD = TRUE;     //SPI 1
    _U1MD = TRUE;       //UART 1
    _T2MD = TRUE;       //Timers 2,3,4 y 5
    _T3MD = TRUE;
    _T4MD = TRUE;
    _T5MD = TRUE;
    _OC1MD = TRUE;      //Output Compare 1,2,3,4 y 5
    _OC2MD = TRUE;
    _OC3MD = TRUE;
    _OC4MD = TRUE;
    _OC5MD = TRUE;
    _IC1MD = TRUE;      //Input Capture 1,2,3,4 y 5
    _IC2MD = TRUE;
    _IC3MD = TRUE;
    _IC4MD = TRUE;
    _IC5MD = TRUE;
    _I2C2MD = TRUE;     //I2C 2
    _CRCPMD = TRUE;     //Módulo CRC
    _CMPMD = TRUE;      //Módulo Comparator
}

//********************************************************************************************************************
//Función que sumerge al sistema en modo de bajo consumo (Power Savings Mode Sleep).
//********************************************************************************************************************
void Modo_Sleep(void)
{
   //Leds OFF indicando que entramos en Sleep
   LEDs_OFF;
   //Limpiamos el LCD
   LCD_Clear();
   //Se configura la UART para forzar el despierte del micro al recibir datos remotos.
   UART2_Configura_Modo_Sleep();
   //Activamos el watchdog para despertar del Sleep, luego lo deshabilitamos
   _SWDTEN = 1;
   Sleep();
   _SWDTEN = 0;
   //Configuración de UART para que no fuerce despierte al recibir datos (Micro ya despierto).
   U2MODEbits.WAKE = 0;
}

//********************************************************************************************************************
//Función que se encarga de procesar las diferentes tareas a realizar tras cada despierte del micro.
//Se determina cual fué la causa del despierte y se realizan las acciones oportunas asociadas.
//********************************************************************************************************************
void Procesa_Evento_Despierte(void)
{
   //DESPIERTE POR CONEXIÓN DE ALIMENTACIÓN//////////////////////////////////////////////////////////////////////////////////
   //Arranque del sistema y mostrado de la pantalla de inicio.
   if(DESP_ALIMENTACION)
   {
       LED3_ON;
       Mensaje_Bienvenida();
   }
   //DESPIERTE POR WATCHDOG DURANTE SLEEP////////////////////////////////////////////////////////////////////////////////////
   //En funcionamineto normal, el micro se despertará por indicación del timer Watchdog, y se realizarán, cuando toque, el
   //muestreo de los sensores y el envío de datos vía modem.
   else if(DESP_WATCHDOG_SLEEP)
   {
       LED4_ON;
       Procesa_Evento_Sensores_Bateria();
   }
   //DESPIERTE POR PULSADOR EXTERNO -> MENÚ OPERARIO//////////////////////////////////////////////////////////////////////////
   //Al pulsar en el bóton de reset externo, se accede al menú operario del sistema.
   else if(DESP_USUARIO)
   {
       LED5_ON;
       Procesa_Evento_Menu();
   }
   //RESET POR BATERIA BAJA (Brown Out)////////////////////////////////////////////////////////////////////////////////////////
   //En caso de que el micro determine que la tensión de alimentación está cayendo a niveles críticos, se ubicará el sistema en
   //modo de recuperación de la batería, ahorrando consumo al máximo y modificando la funcionalidad de medidas y envíos temporalmente.
   else if (RESET_LOW_BAT)
   {
       LED7_ON;
       Procesa_Modo_Recuperacion();
   }
   //DESPIERTE POR RESET SOFTWARE (Fallo de programa) Ó POR WATCHDOG SIN SLEEP Ó POR IDLE//////////////////////////////////////
   else if ((RESET_SOFTWARE) || (RESET_EXCEPCION))
   {
       LED8_ON;
       Procesa_Reset_No_Controlado();
   }
   //DESPIERTE POR UART por descarte tras chequear todos los posibles tipos de despierte////////////////////////////////////////
   //Este caso se produce al despertarse el micro tras recibir datos por la UART mientras está en modo Sleep.
   else
   {
       LED6_ON;
       Procesa_Evento_UART();
   }

   //Limpieza de todas banderas del RCON: POR=0, BOR=0, SLEEP=0, WDTO=0, SWR=0, EXTR=0
   RCON=0;
}
