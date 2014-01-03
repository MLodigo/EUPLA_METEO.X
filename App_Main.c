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
//Funci�n que muestra en el display el mensaje inicial al arrancar el sistema.
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
//Funci�n que se encarga de gestionar el movimiento del usuario a trav�s del men� operario.
//********************************************************************************************************************
void Procesa_Evento_Menu()
{
    MenuOperario();
}

//********************************************************************************************************************
//Funci�n que se encarga de poner al sistema en modo de bajo consumo para fomentar la recuperaci�n de la bater�a.
//********************************************************************************************************************
void Procesa_Modo_Recuperacion()
{
    Inicializacion_Modulo_EEPROM();
    Activar_Modo_Recuperacion();
}

//********************************************************************************************************************
//Funci�n que gestiona el procesamiento de un posible reset por watchdog (fallo en el programa) o 
//por software (excepci�n y trap).
//********************************************************************************************************************
void Procesa_Reset_No_Controlado()
{
    LCD_Clear();
    LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"    ATENCION!   ");
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)" RESET SISTEMA  ");
    Retardo(2000);
}

//********************************************************************************************************************
//Funci�n que atiende a los eventos llegados a trav�s del puerto de comunicaci�nes donde 
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
   //se entiende que la comunicaci�n finaliz� y salimos..
   Reset_Contador_MSeg();
   while(Lectura_Contador_MSeg()<2000)
   {
       if(UART2_DatosPorLeer())
       {
           Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
           for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
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
//Funci�n que realiza las acciones o comandos llegados v�a modem.
//********************************************************************************************************************
void Ejecuta_Peticion_Recibida(char * Comando)
{
    LCD_Clear();
    LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"  Procesando!   ");
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)Comando);
    Retardo(2000);
}

//********************************************************************************************************************
//Funci�n que gestiona las medidas sobre los sensores y el control sobre el nivel de la bater�a.
//Se comprueba si el nivel de la bater�a est� cr�tico, para pasar a modo de recuperaci�n.
//En caso de no estar en modo recuperaci�n, se actualizan los contadores de n�mero de despiertes
//para determinar si ha llegado el momento de realizar un muestreo de los sensores o incluso un
//env�o de datos v�a modem.
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

    //Preparaci�n de la comunicaci�n con la memoria
    Inicializacion_Modulo_EEPROM();

    //Se revisa el nivel de la bater�a
    NIVEL_BATERIA Nivel = Comprobacion_Estado_Bateria();

    //Si la bater�a est� a nivel m�nimo, entramos en modo recuperaci�n
    if(Nivel == NIVEL_BAJO)
    {
       Activar_Modo_Recuperacion();
       LEDs_ON;
       LCD_Clear();
       LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"Bater�a Baja!   ");
       LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Recuperacion ON ");
       Retardo(1500);
    }

    //Si el sistema est� en modo recuperaci�n de la bater�a, comprobamos si se vuelve a modo normal de trabajo.
    if(Si_En_Modo_Recuperacion())
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

        //Como no estamos en modo recuperaci�n, comprobamos si corresponde realizar un scan sobre los sensores.
        if(Si_Realizar_Medidas_Sensores())
        {
           //Lectura y almacenamiento de valores de los sensores
           LCD_Clear();
           LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"Muestreando     ");
           LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Sensores ON..   ");
           Retardo(1500);

           //Contador de despiertes inicializado a cero.
           Reset_CNT_Despiertes();

           //Inicializaci�n de la variable
           Muestra.Nivel_Bateria=0xFFFF;
           Muestra.Pluviometria=0xFFFF;
           Muestra.Temperatura=0xFFFF;
           Muestra.Vel_Aire=0xFFFF;

           //Muestreo de sensores
           Muestra = Barrido_Sensores();

           //Comprobaci�n de que la muestra se tom� correctamente.
           if((Muestra.Nivel_Bateria == 0xFFFF)||(Muestra.Pluviometria == 0xFFFF)||(Muestra.Temperatura == 0xFFFF)||(Muestra.Vel_Aire == 0xFFFF))
           {
               LCD_Clear();
               LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"Error           ");
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"de Muestreo..!  ");
               Retardo(1500);
               return;
           }

           //Obtenemos la siguiente direcci�n libre para ubicar la muestra en EEPROM
           Dir_Sig_E2P_Libre = Siguiente_Direccion_Libre_EEPROM();

           //Comprobaci�n del rebase de la memoria. En caso de superarlo se ajusta a DIR_BASE_MEDIDAS.
           if(Dir_Sig_E2P_Libre < (TAM_MEMORIA_EEPROM - BYTES_POR_MEDIDA))
           {
               //Guardado de la muestra en memoria
               if(Guarda_Muestra_EEPROM(Muestra, Dir_Sig_E2P_Libre))
               {
                  //Incrementamos el n�mero de muestras tomadas
                  Inc_CNT_Muestras_Tomadas();
               }

               //Presentaci�n en display de la �ltima muestra tomada
               Visualiza_Ultima_Muestra_LCD(Muestra);
           }
           //Si la memoria est� llena de medidas, forzamos el env�o al servidor.
           else
           {
               EEPROM_Llena = TRUE;
           }

           //Comprobaci�n si corresponde enviar las muestras via modem.
           if((EEPROM_Llena)||(Si_Realizar_Envio_Muestras_Modem()))
           {
               Cierra_Com_Modem(); //Para no recibir la informaci�n enviada al alimentar el modem.
               MODEM_ON;

               //Cuenta regresiva para inicializaci�n del modem tras alimentaci�n
               LCD_Clear();
               LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"INICIANDO MODEM ");
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Espere..  10 seg");
               for(cnt=9; cnt!=0; cnt--)
               {
                   Retardo(1000);
                   Info[0] = ' ';
                   Info[1] = 0x30 + cnt;
                   LCD_WrString_LinPos(LCD_LINEA2,10,Info);
               }

               //REGISTRO EN LA RED DEL MODEM///////////////////////////////////
               Inicializacion_Com_Modem();
               LCD_Clear();
               LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"REGISTRO RED GSM");
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Buscando modem..");

               //Comprobaci�n si el modem est� en l�nea de comandos
               if(!AT())
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Comunicacion NOK");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Comprobaci�n de la cobertura
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
               //Insercci�n del PIN para red GSM
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
               //Configuraci�n del modem a modo GPRS
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Activar mod.GPRS");
               if(!AT_GPRSMODE('1'))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuraci�n punto de acceso a internet: Servidor
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Conf. Serv. APN ");
               if(!AT_APNSERV((unsigned char*)"movistar.es"))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuraci�n punto de acceso a internet: Usuario
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Conf.Usuario APN");
               if(!AT_APNUN((unsigned char*)"movistar.es"))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuraci�n punto de acceso a internet: Password
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Conf.Passw. APN ");
               if(!AT_APNPW((unsigned char*)""))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuraci�n enlace TCP: Direcci�n IP
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"TCP: Dir.IP Serv");
               if(!AT_TCPSERV((unsigned char*)"cessor.no-ip.biz"))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuraci�n enlace TCP: Puerto
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"TCP: Puerto Serv");
               if(!AT_TCPPORT((unsigned char*)"20000"))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuraci�n enlace TCP: Retardo de transmisi�n
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"TCP: Delay TXT  ");
               if(!AT_TCPTXDELAY((unsigned char*)"100"))
               {
                   LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Fallo!          ");
                   Retardo(5000);
                   MODEM_OFF;
                   return;
               }
               Retardo(1000);
               //Configuraci�n enlace TCP: Cierre de socket mediante caracter ETX
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
               Retardo(1000);
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

               //Env�o de las medidas almacenadas en memoria
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
               //Desconexi�n a internet mediante APN
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
//Funci�n que realiza la lectura de la �ltima muestra tomada en EEPROM y la visualiza en el display LCD.
//********************************************************************************************************************
void Visualiza_Ultima_Muestra_LCD(SENSORES Muestra)
{
    char Informacion[16];
    unsigned char Info_LCD_LineaUP[16];
    unsigned char Info_LCD_LineaDW[16];
    unsigned char * Puntero;
    BYTE i=0;

    //Inicializaci�n de los array LCD
    for(i=0; i<16; i++)
    {
        Info_LCD_LineaUP[i]= ' ';
        Info_LCD_LineaDW[i]= ' ';
    }

    //Montaje de la informaci�n para mostrarlo en el display LCD
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
    //Ajuste de la cadena para que no aparezcan desplazados en caso de variar el n� de cifras
    //Todos datos son como m�ximo de 3 cifras.
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
    //Ajuste de la cadena para que no aparezcan desplazados en caso de variar el n� de cifras
    //Todos datos son como m�ximo de 3 cifras.
    for(i=i; i<3; i++)
    {
        *Puntero = ' ';
        Puntero++;
    }

    //Nivel bater�a
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
//Funci�n que realiza la incializaci�n de los diferentes m�dulos utilizados tras un Reset.
//********************************************************************************************************************
void Inicializa_Sistema(void)
{
    //Inicializaciones de perif�ricos
    LCD_Inicializacion();
    Inicia_Temporizacion();
    UART2_Configura_Abre();

    //Puesta en hora del reloj RTC
    Rtcc_Inicializacion();        //Habilita el reloj secundario y coloca el reloj en reposo
    rtccFechaHora FechaHoraReloj;
    FechaHoraReloj.w[0] = 0x0014; //Anio
    FechaHoraReloj.w[1] = 0x0103; //Mes - Dia
    FechaHoraReloj.w[2] = 0x0420; //Dia Semana - Hora (0-lunes, 1-martes..)
    FechaHoraReloj.w[3] = 0x5000; //Minutos - Segundos
    Rtcc_Configuracion_FechaHora_Reloj(&FechaHoraReloj);
    Rtcc_Activacion();  //Activaci�n del reloj. Habilita tambien la interrupci�n

    //Desactivaci�n del modem. La estrategia ser� que se activar� solamente cuando se env�en
    //las medidas al servidor.
    MODEM_OFF;

    //Deshabilitar m�dulos no utilizados. Eliminar consumo innecesario.
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
    _CRCPMD = TRUE;     //M�dulo CRC
    _CMPMD = TRUE;      //M�dulo Comparator
}

//********************************************************************************************************************
//Funci�n que sumerge al sistema en modo de bajo consumo (Power Savings Mode Sleep).
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
   //Configuraci�n de UART para que no fuerce despierte al recibir datos (Micro ya despierto).
   U2MODEbits.WAKE = 0;
}

//********************************************************************************************************************
//Funci�n que se encarga de procesar las diferentes tareas a realizar tras cada despierte del micro.
//Se determina cual fu� la causa del despierte y se realizan las acciones oportunas asociadas.
//********************************************************************************************************************
void Procesa_Evento_Despierte(void)
{
   //DESPIERTE POR CONEXI�N DE ALIMENTACI�N//////////////////////////////////////////////////////////////////////////////////
   //Arranque del sistema y mostrado de la pantalla de inicio.
   if(DESP_ALIMENTACION)
   {
       LED3_ON;
       Mensaje_Bienvenida();
   }
   //DESPIERTE POR WATCHDOG DURANTE SLEEP////////////////////////////////////////////////////////////////////////////////////
   //En funcionamineto normal, el micro se despertar� por indicaci�n del timer Watchdog, y se realizar�n, cuando toque, el
   //muestreo de los sensores y el env�o de datos v�a modem.
   else if(DESP_WATCHDOG_SLEEP)
   {
       LED4_ON;
       Procesa_Evento_Sensores_Bateria();
   }
   //DESPIERTE POR PULSADOR EXTERNO -> MEN� OPERARIO//////////////////////////////////////////////////////////////////////////
   //Al pulsar en el b�ton de reset externo, se accede al men� operario del sistema.
   else if(DESP_USUARIO)
   {
       LED5_ON;
       Procesa_Evento_Menu();
   }
   //RESET POR BATERIA BAJA (Brown Out)////////////////////////////////////////////////////////////////////////////////////////
   //En caso de que el micro determine que la tensi�n de alimentaci�n est� cayendo a niveles cr�ticos, se ubicar� el sistema en
   //modo de recuperaci�n de la bater�a, ahorrando consumo al m�ximo y modificando la funcionalidad de medidas y env�os temporalmente.
   else if (RESET_LOW_BAT)
   {
       LED7_ON;
       Procesa_Modo_Recuperacion();
   }
   //DESPIERTE POR RESET SOFTWARE (Fallo de programa) � POR WATCHDOG SIN SLEEP � POR IDLE//////////////////////////////////////
   else if ((RESET_SOFTWARE) || (RESET_EXCEPCION))
   {
       LED8_ON;
       Procesa_Reset_No_Controlado();
   }
   //DESPIERTE POR UART por descarte tras chequear todos los posibles tipos de despierte////////////////////////////////////////
   //Este caso se produce al despertarse el micro tras recibir datos por la UART mientras est� en modo Sleep.
   else
   {
       LED6_ON;
       Procesa_Evento_UART();
   }

   //Limpieza de todas banderas del RCON: POR=0, BOR=0, SLEEP=0, WDTO=0, SWR=0, EXTR=0
   RCON=0;
}
