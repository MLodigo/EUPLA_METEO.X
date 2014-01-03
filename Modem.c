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
//                                                M�TODOS PRIVADOS
//********************************************************************************************************************
//********************************************************************************************************************


//********************************************************************************************************************
//********************************************************************************************************************
//                                                M�TODOS P�BLICOS
//********************************************************************************************************************
//********************************************************************************************************************

//********************************************************************************************************************
//Funcion que inicializa los m�dulos que intervienen el proceso de comunicaci�n con el m�dem.
//********************************************************************************************************************
void Inicializacion_Com_Modem(void)
{
    Inicia_Temporizacion();
    UART2_Configura_Abre();
}

//********************************************************************************************************************
//Funcion que cierra la comunicaci�n con el modem
//********************************************************************************************************************
void Cierra_Com_Modem(void)
{
    UART2_Cierra();
}

//********************************************************************************************************************
//Funcion que comprueba que el modem se encuentra en l�nea y listo para recibir comandos AT.
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

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasan un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funcion ejectua el comando AT+CREG? para comprobar el estado de registro del m�dem en la red m�vil GSM/GPRS
//La respuesta ser� +CREG: <mode>,<stat><CR><LF><CR><LF>OK<CR><LF> donde mode=1 y stat=1 para un registro correcto.
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

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasan un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while(Lectura_Contador_MSeg()<300)
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer
           }
       }

       //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
       for(i=0; i<100; i++) 
       { 
           if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'K'))
           {
               OK_Recibido = TRUE;
               break;
           } 
       }
       //Si est� el OK en la respuesta, buscamos la secuencia 1,1 que indica registrado correctamente en la red m�vil.
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
//Funci�n que realiza el trabajo de registrar en la red GSM/GPRS al modem insertando el PIN de desbloqueo de la
//tarjeta SIM. La respuesta ser� OK precedido de un retardo, que es lo que cuesta al dispositivo finalizar el proceso.
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

   //Montaje de la informaci�n del comando AT
   strcat((char *)COM_AT, (const char *)pin);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que realiza el trabajo de habilitar o deshabilitar la apliacion embebida OpenAT que los modems Wavecom llevan
//instalada, y que se trata concretamente de la pila TCP/IP para realizar conexiones a servidores a trav�s de internet.
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

   //Montaje de la informaci�n del comando AT
   COM_AT[9]=valor;
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que habilita o deshabilita el perfil de trabajo en modo GPRS del modem Wavecom.
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

   //Montaje de la informaci�n del comando AT
   COM_AT[12]=valor;
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que configura el nombre del punto de acceso (APN) o proveedor de internet.
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

   //Montaje de la informaci�n del comando AT
   strcat((char *)COM_AT, (const char *)proveedor);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que configura el nombre usuario para acceder punto de acceso (APN) o proveedor de internet.
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

   //Montaje de la informaci�n del comando AT
   strcat((char *)COM_AT, (const char *)usuario);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que configura el password para acceder punto de acceso (APN) o proveedor de internet.
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

   //Montaje de la informaci�n del comando AT
   strcat((char *)COM_AT, (const char *)password);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que configura en el modem la direcci�n IP del centro servidor a trav�s de su DNS.
//Se puede utilizar una cuenta (p.e. NO-IP) de gesti�n de cambios de IP din�micas.
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

   //Montaje de la informaci�n del comando AT
   strcat((char *)COM_AT, (const char *)DNS_Servidor);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que configura en el modem el puerto del centro servidor sobre el que est� ubicado el servicio web que
//recoge la informaci�n enviada.
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

   //Montaje de la informaci�n del comando AT
   strcat((char *)COM_AT, (const char *)puerto);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que configura el tiempo que debe transcurrir antes de enviar una trama TCP que no ha sido enteramente
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

   //Montaje de la informaci�n del comando AT
   strcat((char *)COM_AT, (const char *)retardo);
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que configura si el car�cter ETX tien significado de final de conexi�n y pasar a modo de comandos desde
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

   //Montaje de la informaci�n del comando AT
   COM_AT[11]=valor;
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que realiza el enlace o desenlace al servicio GPRS. Se realiza un "Attach" o "Detach" a dicho servicio, y
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

   //Montaje de la informaci�n del comando AT
   COM_AT[9]=valor;
   strcat((char *)COM_AT, (const char *)COM_AT_Fin);

   //Bucle para realizar reintentos
   do
   {
       //Inicializamos buffer de datos
       for(i=0; i<100; i++) {DatosRecibidos_UART2[i] = '\0';}
       if(UART2_DatosPorLeer()){UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());};

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
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
//Funci�n que inicia una conexi�n al servidor remoto una vez que se ha configurado el modem en modo
//GPRS y se ha realizado un "Attach" a este servicio. La respuesta ser� la direcci�n IP asignada al modem y la
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

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK_Info"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'k')&&(DatosRecibidos_UART2[i+2] == '_')&&
                      (DatosRecibidos_UART2[i+3] == 'I')&&(DatosRecibidos_UART2[i+4] == 'n')&&(DatosRecibidos_UART2[i+5] == 'f')&&(DatosRecibidos_UART2[i+6] == 'o'))
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
//Funci�n que realiza una desconexi�n al servidor remoto una vez que se ha configurado el modem en modo
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

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasan 2 segundos sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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
//Funci�n que env�a el comando AT que permite abrir un socket sobre servidor y puerto configurado previamente.
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

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK_Info"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'k')&&(DatosRecibidos_UART2[i+2] == '_')&&
                      (DatosRecibidos_UART2[i+3] == 'I')&&(DatosRecibidos_UART2[i+4] == 'n')&&(DatosRecibidos_UART2[i+5] == 'f')&&(DatosRecibidos_UART2[i+6] == 'o'))
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
//Funci�n que env�a el car�cter ETC (0x03). Este car�cter es utilizado cuando una vez abierto el socket con el
//servidor, se desea terminar finalizar la conexi�n, pasando de "modo datos" a "modo comandos", con lo que el modem
//comenzar� a responder a los comandos AT.
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

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasa un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<30000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK_Info"
               for(i=0; i<100; i++)
               {
                   if((DatosRecibidos_UART2[i] == 'O')&&(DatosRecibidos_UART2[i+1] == 'k')&&(DatosRecibidos_UART2[i+2] == '_')&&
                      (DatosRecibidos_UART2[i+3] == 'I')&&(DatosRecibidos_UART2[i+4] == 'n')&&(DatosRecibidos_UART2[i+5] == 'f')&&(DatosRecibidos_UART2[i+6] == 'o'))
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
//Funci�n que comprueba la calidad de la se�al recibida por el modem. La cobertura viene determinada por el baremo:
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

       //Env�o del comando
       UART2_Envia_Cadena(COM_AT);

       //Esperamos a recibir todos los datos.. Si pasan un tiempo sin recibir nada,
       //se entiende que la comunicaci�n finaliz� y salimos..
       Reset_Contador_MSeg();
       while((Lectura_Contador_MSeg()<5000)&&(!OK_Recibido))
       {
           if(UART2_DatosPorLeer())
           {
               Reset_Contador_MSeg();                                             //Reset del contador de tiempo de salida del bucle
               for(i=0; i<100; i++) { DatosRecAux[i] = '\0'; }                    //Inicializamos buffer recepci�n parcial
               UART2_LeerDatos((char *)DatosRecAux, UART2_DatosPorLeer());        //Lectura de datos del buffer UART
               strcat((char *)DatosRecibidos_UART2, (const char *)DatosRecAux);   //Concatenamos datos recibidos en el buffer

               //Buscamos en la respuesta recibida si el comando finaliz� con �xito buscando "OK"
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

   //Averiguamos el valor de la intensidad de la se�al y devolvemos el resultado
   if(OK_Recibido)
   {
       //Obtenci�n del valor num�rico de la intensidad de la se�al
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

       //Interpretaci�n del valor obtenido
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


