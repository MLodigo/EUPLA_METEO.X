#include "App_Main.h"
#include "Temporizacion.h"
#include "PMP_LCD.h"
#include "Gestor_EEPROM.h"
#include "Switches.h"
#include "leds.h"
#include "Gestor_Bateria.h"
#include "string.h"
#include "Gestor_Sensores.h"
#include "Rele.h"
#include "MenuOperario.h"


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

//Función que muestra en el display el mensaje inicial.
void Mensaje_Bienvenida()
{
    LCD_Clear();
    LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"EUPLA-METEO '14 ");
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"      v1.0      ");
    Retardo(2000);
    LCD_Clear();
}

//Función que se encarga de gestionar el movimiento del usuario a través del menú del sistema
void Procesa_Evento_Menu()
{
    MenuOperario();
}

//Función que se encarga de poner al sistema en modo de bajo consumo para fomentar la recuperación de la batería.
void Procesa_Modo_Recuperacion()
{
    Inicializacion_Modulo_EEPROM();
    Activar_Modo_Recuperacion();
}

//Función que gestiona el procesamiento de un posible reset por watchdog (fallo en el programa) o por software (menú operario) o por salida del modo Idle.
void Procesa_Reset_No_Controlado()
{
    LCD_Clear();
    LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"    ATENCION!   ");
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)" RESET SISTEMA  ");
    Retardo(2000);
}

//Función que atiende a los eventos llegados a través del puerto de comunicaciónes con el modem.
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

   //Esperamos a recibir todos los datos.. Si pasan 2 segundos sin recibir nada, salimos..
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

//Función que realiza las acciones o comandos llegados vía modem.
void Ejecuta_Peticion_Recibida(char * Comando)
{
    LCD_Clear();
    LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"  Procesando!   ");
    LCD_WriteLinea(LCD_LINEA2, (unsigned char*)Comando);
    Retardo(2000);
}

//Función que gestiona las medidas sobre los sensores y el control sobre el nivel de la batería.
void Procesa_Evento_Sensores_Bateria()
{
    BYTE Reintentos = 0;
    
    //Preparación de la comunicación con la memoria
    Inicializacion_Modulo_EEPROM();

    //Si el sistema está en modo recuperación de la batería, volvemos a comprobar el nivel para ver si se vuelve a modo normal de trabajo.
    if(Si_En_Modo_Recuperacion())
    {
        if(Comprobacion_Estado_Bateria()== NIVEL_BAJO)
        {
            LEDs_ON;
            Retardo(100);
        }
        else
        {
            Desactivar_Modo_Recuperacion();
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
           Reintentos=0;
           while((Lectura_Sensores_Guarda_Muestra_EEPROM()==FALSE)&&(Reintentos<3))
           {
               Reintentos++;
           }
           if(Reintentos==3){return;}

           //Contador de despiertes inicializado a cero.
           Reset_CNT_Despiertes();

           //Incrementamos el número de muestras tomadas
           Inc_CNT_Muestras_Tomadas();

           //Comprobación si corresponde enviar las muestras via modem.
           if(Si_Realizar_Envio_Muestras_Modem())
           {
               LCD_Clear();
               LCD_WriteLinea(LCD_LINEA1, (unsigned char*)"MODEM HABILITADO");
               LCD_WriteLinea(LCD_LINEA2, (unsigned char*)"Enviando Datos..");
               Retardo(1000);
               Enviar_Muestras_Modem();
               Reset_CNT_Muestras_Tomadas();
           }
        }
    }
}

//********************************************************************************************************************
//********************************************************************************************************************
//                                                METODOS PUBLICOS
//********************************************************************************************************************
//********************************************************************************************************************

//Función que realiza la incialización de los diferentes módulos utilizados tras un Reset.
void Inicializa_Sistema(void)
{
    //Inicializaciones de periféricos
    LCD_Inicializacion();
    Inicia_Temporizacion();
    UART2_Configura_Abre();

    //Puesta en hora del reloj RTC
    Rtcc_Inicializacion();        //Habilita el reloj secundario y coloca el reloj en reposo
    rtccFechaHora FechaHoraReloj;
    FechaHoraReloj.w[0] = 0x0013; //Anio
    FechaHoraReloj.w[1] = 0x1121; //Mes - Dia
    FechaHoraReloj.w[2] = 0x0300; //Dia Semana - Hora
    FechaHoraReloj.w[3] = 0x2700; //Minutos - Segundos
    Rtcc_Configuracion_FechaHora_Reloj(&FechaHoraReloj);
    Rtcc_Activacion();  //Activación del reloj. Habilita tambien la interrupción

    //Activación del modem
    MODEM_ON;

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

//Función que sumerge al sistema en modo de bajo consumo (Power Savings Mode Sleep)
void Modo_Sleep(void)
{
   //Leds OFF indicando que entramos en Sleep
   LEDs_OFF;
   //Limpiamos el LCD
   LCD_Clear();
   //Activamos el watchdog para despertar del Sleep, luego lo deshabilitamos
   UART2_Configura_Modo_Sleep();
   _SWDTEN = 1;
   Sleep();
   _SWDTEN = 0;
   U2MODEbits.WAKE = 0;
}

//Función que se encarga de procesar las diferentes tareas a realizar tras cada despierte del micro.
void Procesa_Evento_Despierte(void)
{
    //DESPIERTE POR CONEXIÓN DE ALIMENTACIÓN//////////////////////////////////////////////////////////////////////////////////
   if(DESP_ALIMENTACION)
   {
       LED3_ON;
       Mensaje_Bienvenida();
   }
   //DESPIERTE POR WATCHDOG DURANTE SLEEP////////////////////////////////////////////////////////////////////////////////////
   else if(DESP_WATCHDOG_SLEEP)
   {
       LED4_ON;
       Procesa_Evento_Sensores_Bateria();
   }
   //DESPIERTE POR PULSADOR EXTERNO -> MENÚ OPERARIO//////////////////////////////////////////////////////////////////////////
   else if(DESP_USUARIO)
   {
       LED5_ON;
       Procesa_Evento_Menu();
   }
   //RESET POR BATERIA BAJA (Brown Out)
   else if (RESET_LOW_BAT)
   {
       LED7_ON;
       Procesa_Modo_Recuperacion();
   }
   //DESPIERTE POR RESET SOFTWARE DEL MENÚ OPERARIO (Opción "Reset Dispositivo") Ó POR WATCHDOG SIN SLEEP Ó POR IDLE//////////
   else if ((RESET_SOFTWARE) || (RESET_EXCEPCION))
   {
       LED8_ON;
       Procesa_Reset_No_Controlado();
   }
   //DESPIERTE POR UART por descarte tras chequear todos los posibles tipos de despierte
   else
   {
       LED6_ON;
       Procesa_Evento_UART();
   }

   //Limpieza de todas banderas del RCON: POR=0, BOR=0, SLEEP=0, WDTO=0, SWR=0, EXTR=0
   RCON=0;
}











