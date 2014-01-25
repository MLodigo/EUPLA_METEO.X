#include "Modem.h"
#include "UART.h"
#include "Temporizacion.h"
#include <string.h>

//********************************************************************************************************************
//********************************************************************************************************************
//                                                VARIABLES MIEMBRO
//********************************************************************************************************************
//********************************************************************************************************************


//********************************************************************************************************************
//********************************************************************************************************************
//                                                MÉTODOS PRIVADOS
//********************************************************************************************************************
//********************************************************************************************************************


//********************************************************************************************************************
//********************************************************************************************************************
//                                                MÉTODOS PÚBLICOS
//********************************************************************************************************************
//********************************************************************************************************************

//********************************************************************************************************************
//Funcion que inicializa los módulos que intervienen el proceso de comunicación con el módem.
//********************************************************************************************************************
void Inicializacion_Com_Modem(void)
{
    Inicia_Temporizacion();
    UART2_Configura_Abre();
}

//********************************************************************************************************************
//Funcion que cierra la comunicación con el modem
//********************************************************************************************************************
void Cierra_Com_Modem(void)
{
    UART2_Cierra();
}

//********************************************************************************************************************
//Funcion que comprueba que el modem se encuentra en línea y listo para recibir comandos AT.
//********************************************************************************************************************
BOOL AT(void)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[5] = {'A','T',0x0D,0x0A,'\0'};

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasan un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Funcion ejectua el comando AT+CREG? para comprobar el estado de registro del módem en la red móvil GSM/GPRS
//La respuesta será +CREG: <mode>,<stat><CR><LF><CR><LF>OK<CR><LF> donde mode=1 y stat=1 para un registro correcto.
//********************************************************************************************************************
BOOL AT_CREG(void)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   BOOL Resultado = FALSE;
   char COM_AT[11] = {'A','T','+','C','R','E','G','?',0x0D,0x0A,'\0'};
   
   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasan un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while(Lectura_Contador_MSeg()<1000)
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer
           }
       }

       //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
       for(i=0; i<100; i++) 
       { 
           if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
           {
               OK_Recibido = TRUE;
               break;
           } 
       }
       //Si está el OK en la respuesta, buscamos la secuencia 1,1 que indica registrado correctamente en la red móvil.
       if(OK_Recibido)
       {
           for(i=0; i<100; i++) 
           { 
               if((DatosRecibidos_UART2[i] == '1')&&(DatosRecibidos_UART2[i+1] == ',')&&(DatosRecibidos_UART2[i+2] == '1'))
               {
                   Resultado = TRUE;
                   break;
               } 
           }
       }
       
       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return Resultado;
}

//********************************************************************************************************************
//Función que realiza el trabajo de registrar en la red GSM/GPRS al modem insertando el PIN de desbloqueo de la
//tarjeta SIM. La respuesta será OK precedido de un retardo, que es lo que cuesta al dispositivo finalizar el proceso.
//********************************************************************************************************************
BOOL AT_CPIN(unsigned char* pin)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[15] = {'A','T','+','C','P','I','N','=','\0','\0','\0','\0','\0','\0','\0'};
   char COM_AT_Fin[3] = {0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   strcat((char *)COM_AT, (const char *)pin);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK" o "+CME ERROR: 3" (Pin ya insertado)
               for(i=0; i<100; i++)
               {
                   if(((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))||
                      ((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'R')&&(DatosRecibidos_UART2[i+2] == ':')&&(DatosRecibidos_UART2[i+3] == ' ')&&(DatosRecibidos_UART2[i+4] == '3')))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que envía el mensaje indicado vía SMS. Ojo! el modem debe estar perfectamente configurado previamente.
//********************************************************************************************************************
BOOL AT_CMGS(unsigned char* mensaje)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL MAYOR_QUE_Recibido = FALSE;
   BOOL OK_Recibido = FALSE;
   char COM_AT_SMS_I[24] = {'A','T','+','C','M','G','S','=','"','+','3','4','6','7','4','9','5','7','2','3','7','"',0x0D,'\0'}; //Fran
   char COM_AT_SMS_II[60];
   BOOL Resultado = FALSE;

   //Inicialización del array mensaje
   for (i=0; i<60; i++){COM_AT_SMS_II[i]='\0';}

   //Montaje de la información del comando AT
   strcat((char *)COM_AT_SMS_II, (const char *)mensaje);
   
   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío de la primera parte del comando
       UART2_Envia_Cadena(COM_AT_SMS_I);

       //Esperamos a recibir el caracter '>'..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!MAYOR_QUE_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando ">"
               for(i=0; i<100; i++)
               {
                   if(DatosRecibidos_UART2[i] == '>')
                   {
                       MAYOR_QUE_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((MAYOR_QUE_Recibido == FALSE)&&(Reintentos < 3));


   //Si finalizó correctamente la primera parte del proceso, se continúa con el envío del mensaje..
   if(MAYOR_QUE_Recibido)
   {
       Reintentos = 0;
       //Bucle para realizar reintentos
       do
       {
           //Inicializamos buffer de datos
           for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
           if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

           //Envío de la primera parte del comando
           UART2_Envia_Cadena(COM_AT_SMS_II);
           UART2_Envia_Byte(0x1A);

           //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
           //se entiende que la comunicación finalizó y salimos..
           Reset_Contador_MSeg();
           while((Lectura_Contador_MSeg()<4000)&&(!OK_Recibido))
           {
               if(UART2_DatosPorLeer())
               {
                   Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
                   for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
                   UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
                   strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

                   //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
                   for(i=0; i<100; i++)
                   {
                       if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                       {
                           OK_Recibido = TRUE;
                           Resultado = TRUE;
                           break;
                       }
                   }
               }
           }

           //Se incrementan los reintentos
           Reintentos++;
        }while((OK_Recibido == FALSE)&&(Reintentos < 3));
    }

   //Final del proceso
   return Resultado;
}

//********************************************************************************************************************
//Función que realiza el trabajo de habilitar o deshabilitar la apliacion embebida OpenAT que los modems Wavecom llevan
//instalada, y que se trata concretamente de la pila TCP/IP para realizar conexiones a servidores a través de internet.
//********************************************************************************************************************
BOOL AT_WOPEN(unsigned char valor)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[13] = {'A','T','+','W','O','P','E','N','=','\0','\0','\0','\0'};
   char COM_AT_Fin[3] = {0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   COM_AT[9]=valor;
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que habilita o deshabilita el perfil de trabajo en modo GPRS del modem Wavecom.
//********************************************************************************************************************
BOOL AT_GPRSMODE(unsigned char valor)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[16] = {'A','T','#','G','P','R','S','M','O','D','E','=','\0','\0','\0','\0'};
   char COM_AT_Fin[3] = {0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   COM_AT[12]=valor;
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que configura el nombre del punto de acceso (APN) o proveedor de internet.
//********************************************************************************************************************
BOOL AT_APNSERV(unsigned char* proveedor)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[27] = {'A','T','#','A','P','N','S','E','R','V','=','"','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
   char COM_AT_Fin[4] = {'"',0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   strcat((char *)COM_AT, (const char *)proveedor);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que configura el nombre usuario para acceder punto de acceso (APN) o proveedor de internet.
//********************************************************************************************************************
BOOL AT_APNUN(unsigned char* usuario)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[25] = {'A','T','#','A','P','N','U','N','=','"','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
   char COM_AT_Fin[4] = {'"',0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   strcat((char *)COM_AT, (const char *)usuario);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que configura el password para acceder punto de acceso (APN) o proveedor de internet.
//********************************************************************************************************************
BOOL AT_APNPW(unsigned char* password)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[25] = {'A','T','#','A','P','N','P','W','=','"','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
   char COM_AT_Fin[4] = {'"',0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   strcat((char *)COM_AT, (const char *)password);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que configura en el modem la dirección IP del centro servidor a través de su DNS.
//Se puede utilizar una cuenta (p.e. NO-IP) de gestión de cambios de IP dinámicas.
//********************************************************************************************************************
BOOL AT_TCPSERV(unsigned char* DNS_Servidor)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[32] = {'A','T','#','T','C','P','S','E','R','V','=','"','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
   char COM_AT_Fin[4] = {'"',0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   strcat((char *)COM_AT, (const char *)DNS_Servidor);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que configura en el modem el puerto del centro servidor sobre el que está ubicado el servicio web que
//recoge la información enviada.
//********************************************************************************************************************
BOOL AT_TCPPORT(unsigned char* puerto)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[19] = {'A','T','#','T','C','P','P','O','R','T','=','\0','\0','\0','\0','\0','\0','\0','\0'};
   char COM_AT_Fin[3] = {0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   strcat((char *)COM_AT, (const char *)puerto);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que configura el tiempo que debe transcurrir antes de enviar una trama TCP que no ha sido enteramente
//completada con los datos de usuario.
//********************************************************************************************************************
BOOL AT_TCPTXDELAY(unsigned char* retardo)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[20] = {'A','T','#','T','C','P','T','X','D','E','L','A','Y','=','\0','\0','\0','\0','\0','\0'};
   char COM_AT_Fin[3] = {0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   strcat((char *)COM_AT, (const char *)retardo);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que configura si el carácter ETX tien significado de final de conexión y pasar a modo de comandos desde
//modo datos.
//********************************************************************************************************************
BOOL AT_DLEMODE(unsigned char valor)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[15] = {'A','T','#','D','L','E','M','O','D','E','=','\0','\0','\0','\0'};
   char COM_AT_Fin[3] = {0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   COM_AT[11]=valor;
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que realiza el enlace o desenlace al servicio GPRS. Se realiza un "Attach" o "Detach" a dicho servicio, y
//es requisito necesario previo a establecer un socket TCP con el servidor remoto.
//********************************************************************************************************************
BOOL AT_CGATT(unsigned char valor)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[13] = {'A','T','+','C','G','A','T','T','=','\0','\0','\0','\0'};
   char COM_AT_Fin[3] = {0x0D,0x0A,'\0'};

   //Montaje de la información del comando AT
   COM_AT[9]=valor;
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //En caso de hacer attach al servicio GPRS, buscamos en la respuesta "GREG: 1" y con detach "GREG: 0"
               for(i=0; i<100; i++)
               {
                   //if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                  if((DatosRecibidos_UART2[i] == 'G')&&(DatosRecibidos_UART2[i+1] == 'R')&&(DatosRecibidos_UART2[i+2] == 'E')&&(DatosRecibidos_UART2[i+3] == 'G')&&
                      (DatosRecibidos_UART2[i+4] == ':')&&(DatosRecibidos_UART2[i+5] == ' ')&&(DatosRecibidos_UART2[i+6] == valor))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }

               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que comprueba si se ha realizado el attach al servicio GPRS. Devuelve ERROR hasta que se finaliza el proceso
//que es cuando devuelve OK
//********************************************************************************************************************
BOOL AT_CNUM(void)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[12] = {'A','T','+','C','N','U','M','=','?',0x0D,0x0A,'\0'};

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<2000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //En caso de estar attached al servicio GPRS, lo indica la respuesta "OK" si no
               for(i=0; i<100; i++)
               {
                  if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }

               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que inicia una conexión al servidor remoto una vez que se ha configurado el modem en modo
//GPRS y se ha realizado un "Attach" a este servicio. La respuesta será la dirección IP asignada al modem y la
//cadena "OK_Info_GprsActivation".
//********************************************************************************************************************
BOOL AT_CONNECTIONSTART(void)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[21] = {'A','T','#','C','O','N','N','E','C','T','I','O','N','S','T','A','R','T',0x0D,0x0A,'\0'};

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "GprsActivation"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'G')&&(DatosRecibidos_UART2[i+1] == 'p')&&(DatosRecibidos_UART2[i+2] == 'r')&&(DatosRecibidos_UART2[i+3] == 's')&&
                      (DatosRecibidos_UART2[i+4] == 'A')&&(DatosRecibidos_UART2[i+5] == 'c')&&(DatosRecibidos_UART2[i+6] == 't')&&(DatosRecibidos_UART2[i+7] == 'i')&&
                      (DatosRecibidos_UART2[i+8] == 'v')&&(DatosRecibidos_UART2[i+9] == 'a')&&(DatosRecibidos_UART2[i+10] == 't')&&(DatosRecibidos_UART2[i+11] == 'i')&&
                      (DatosRecibidos_UART2[i+12] == 'o')&&(DatosRecibidos_UART2[i+13] == 'n'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que realiza una desconexión al servidor remoto una vez que se ha configurado el modem en modo
//GPRS y se ha realizado un "Attach" a este servicio.
//********************************************************************************************************************
BOOL AT_CONNECTIONSTOP(void)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[20] = {'A','T','#','C','O','N','N','E','C','T','I','O','N','S','T','O','P',0x0D,0x0A,'\0'};

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasan 2 segundos sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que envía el comando AT que permite abrir un socket sobre servidor y puerto configurado previamente.
//********************************************************************************************************************
BOOL AT_OTCP(void)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[10] = {'A','T','#','O','T','C','P',0x0D,0x0A,'\0'};

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "ForData"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'F')&&(DatosRecibidos_UART2[i+1] == 'o')&&(DatosRecibidos_UART2[i+2] == 'r')&&
                      (DatosRecibidos_UART2[i+3] == 'D')&&(DatosRecibidos_UART2[i+4] == 'a')&&(DatosRecibidos_UART2[i+5] == 't')&&(DatosRecibidos_UART2[i+6] == 'a'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que envía el carácter ETC (0x03). Este carácter es utilizado cuando una vez abierto el socket con el
//servidor, se desea terminar finalizar la conexión, pasando de "modo datos" a "modo comandos", con lo que el modem
//comenzará a responder a los comandos AT.
//********************************************************************************************************************
BOOL Caracter_ETX(void)
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[2] = {0x03,'\0'};

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Final del proceso
   return OK_Recibido;
}

//********************************************************************************************************************
//Función que comprueba la calidad de la señal recibida por el modem. La cobertura viene determinada por el baremo:
//
//  0: -113 dBm o menor.
//  1: -111 dBm.
//  2 a 30: -109 a ?53 dBm.
//  31: -51dBm o mayor.
//  99: No destectada o inexistente.
//
//  Respuesta ejemplo al comando:
//        +CSQ: 21,0<CR><LF>
//        <CR><LF>
//        OK<CR><LF
//********************************************************************************************************************
CALIDAD_COBERTURA AT_CSQ()
{
   BYTE DatosRecibidos_UART2[100];
   BYTE DatosRecAux[100];
   BYTE i = 0;
   BYTE Reintentos = 0;
   BOOL OK_Recibido = FALSE;
   char COM_AT[9] = {'A','T','+','C','S','Q',0x0D,0x0A,'\0'};
   CALIDAD_COBERTURA Intensidad = INTENSIDAD_NULA;
   BYTE Valor_Cobertura = 0;
   BOOL Flag = FALSE;
   BYTE Cifra1 = 255, Cifra2 = 255;

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}

       //Envío del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasan un tiempo sin recibir nada,
       //se entiende que la comunicación finalizó y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepción parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finalizó con éxito buscando "OK"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
                   {
                       OK_Recibido = TRUE;
                       break;
                   }
               }
           }
       }

       //Se incrementan los reintentos
       Reintentos++;
   }while((OK_Recibido == FALSE)&&(Reintentos < 3));

   //Averiguamos el valor de la intensidad de la señal y devolvemos el resultado
   if(OK_Recibido)
   {
       //Obtención del valor numérico de la intensidad de la señal
       Valor_Cobertura = 0;
       for(i=0; i<100; i++)
       {
           if(DatosRecibidos_UART2[i] == ':'){Flag = TRUE;}
           if(DatosRecibidos_UART2[i] == ','){Flag = FALSE; break;}
           if(Flag)
           {
               if((DatosRecibidos_UART2[i]>0x2F)&&(DatosRecibidos_UART2[i]<0x3A))
               {
                   if(Cifra1==255){Cifra1 = DatosRecibidos_UART2[i];}
                   else{Cifra2 = DatosRecibidos_UART2[i];}
               }
           }
       }

       //Valor de cobertura de una sola cifra
       if((Cifra1 != 255)&&(Cifra2 == 255))
       {
           Valor_Cobertura = Cifra1 - 0x30;
       }
       //Valor de cobertura de dos cifras
       else if((Cifra1 != 255)&&(Cifra2 != 255))
       {
           Valor_Cobertura = ((Cifra1 - 0x30)*10)+(Cifra2 -0x30);
       }

       //Interpretación del valor obtenido
       if(Valor_Cobertura<5)
       {
           Intensidad = INTENSIDAD_MUY_BAJA;
       }
       else if((Valor_Cobertura>=5)&&(Valor_Cobertura<10))
       {
           Intensidad = INTENSIDAD_BAJA;
       }
       else if((Valor_Cobertura>=10)&&(Valor_Cobertura<31))
       {
           Intensidad = INTENSIDAD_BUENA;
       }
       else if((Valor_Cobertura>=31)&&(Valor_Cobertura<99))
       {
           Intensidad = INTENSIDAD_EXCELENTE;
       }
       else 
       {
           Intensidad = INTENSIDAD_NULA;
       }
   }

   //Resultado final
   return Intensidad;
}


