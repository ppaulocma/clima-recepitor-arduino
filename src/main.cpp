#include <RF24.h>

//*************** Controle do RF ***********************
#define radioID 0   //Informar "0" para um dispositivo e "1" para o outro dispositivo

struct estruturaDadosRF
{
   boolean ligando = false;   //Esta variavel será usada para solicitar os dados do outro aparelho. Será útil quando o aparelho solicitante esta sendo ligado, para manter os valores do aparelho que já esta ligado.
   float umidade;
   float temperaturaDHT;
   float temperatura;
   float pressao;
};
typedef struct estruturaDadosRF tipoDadosRF;
tipoDadosRF dadosRF;
tipoDadosRF dadosRecebidos;

boolean transmitido = true;
boolean alterado = false;

RF24 radio(7,8);

byte enderecos[][6] = {"1node","2node"};



//*************** Controle do Projeto LOCAL ************
#define intervalo 700
unsigned long delayIntervalo;

void setup() {
   Serial.begin(9600);
  //*************** Controle do RF ***********************
   radio.begin();
  
   #if radioID == 0
      radio.openWritingPipe(enderecos[0]);
      radio.openReadingPipe(1, enderecos[1]);
   #else
      radio.openWritingPipe(enderecos[1]);
      radio.openReadingPipe(1, enderecos[0]);
   #endif

  //Solicita os dados do outro aparelho, se ele estiver ligado. Tenta a comunicação por 2 segundos.
   dadosRF.ligando = true;
   radio.stopListening();                                   
   long tempoInicio = millis();
   while ( !radio.write( &dadosRF, sizeof(tipoDadosRF) ) ) {
      if ((millis() - tempoInicio) > 2000) {
         break;
      }   
   }
   dadosRF.ligando = false; 
   radio.startListening();  

}


void loop() {
  //*************** Controle do RF ***********************
  // se houve alteração dos dados, envia para o outro radio 
   if (alterado || !transmitido) {
      radio.stopListening();                                   
      transmitido = radio.write( &dadosRF, sizeof(tipoDadosRF) );
      radio.startListening();  
      alterado = false;
   };

  //verifica se esta recebendo mensagem       
   if (radio.available()) {                                   
      radio.read( &dadosRecebidos, sizeof(tipoDadosRF) ); 

     //verifica se houve solicitação de envio dos dados
      if (dadosRecebidos.ligando) {
        alterado = true;
      } else {
        dadosRF = dadosRecebidos;
      }
   };


   if ( (millis() - delayIntervalo) > intervalo ) {

      Serial.print(dadosRF.umidade);

      //Serial.print(dadosRF.temperatura);

      Serial.print(dadosRF.temperaturaDHT);

      Serial.print(dadosRF.pressao);

      delayIntervalo = millis();
   };

  delay(10);
}