//Conexoes:
//O SX127x Arduino Shield Dragino v.1.4 deve estar devidamente conectado ao Arduino
//O botao deve estar conectado ao pin 7 

//Includes para a utilizacao da RadioHead
#include <SPI.h>
#include <RH_RF95.h>

//Criação de variáveis globais
RH_RF95 rf95;                        //Instancia do modulo de transmissao
int S, F;                            //Variaveis para a contagem de Sucessos e Falhas
int led = 8;                         //Led utilizado pelo Shield para sinalizar comunicacao
int sf[] = { 7, 8, 9, 10, 11, 12}; //Valores do Spreading Factor
int cr[] = {5, 8};                   //Valores do Coding Rate
int i = 0, j = 0;                    //Variaveis para caminharmos pelo sf e cr
char numero_retransmissao = '1';      //Número da estação de retransmissão para impedir loop com múltiplas estações
                                      // O número da estação deve estar entre 1 e 9, se acabarem os números, comece outra vez do 1
void setup() {
  //Configuracao do led
  pinMode(led, OUTPUT); 
  
  //Inicializa a serial do Arduino
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  
  //Inicialização do modulo
  if (!rf95.init()){//Caso o módulo falhe a inicialização
    Serial.println("Falha na inicialização, verifique o equipamento");
    while(1);
  } 
  Serial.println("Módulo Inicializado com sucesso!");
  rf95.setFrequency(915.0);
  rf95.setSignalBandwidth(125000);
  rf95.setTxPower(13);

  //Fim do Set Up
  Serial.println("Tudo certo! Para inicializar a captura, pressione o botão!");
  //Espera o botao ser pressionado para prosseguir a execucao
  //while(!digitalRead(7));
  //delay(300);

}

void loop() {
  //(Re)Inicializacao dos valores das variaveis
  F = 0;
  S = 0;  

  //Configuracao dos valores de Spreading Factor e Coding Rate
  rf95.setSpreadingFactor(sf[i]);
  Serial.println("Spreading Factor definido para " + String(sf[i]) + ".");
  
  rf95.setCodingRate4(cr[j]);//  fração com denominador de 5 até 8
  Serial.println("Coding Rate definido para " + String(cr[j]) + "."); 

  //Realiza a captura para a combinacao dos parametros ate o botao ser pressionado
  Serial.println("Inicializando captura, aperta o botão para encerrar.");
  while(!digitalRead(7)){
    receiveMessage();
  }

  //Log de quantas mensagens foram recebidas e quantas foram captadas, mas falharam em ser recebidas
  Serial.println("Captura finalizada. Para essa sequencia foram recebidas " + String(S) + " mensagens e " + String(F) + " mensagens falharam.");
  iteracao();
  delay(300);
}

//Funcao que caminha pela combinacao dos valores do Spreading Factor e Coding Rate
void iteracao(){
  if(i == 6 && j == 1){
    Serial.println("Fim da Bateria de Testes!");
    while(1);
  }
  if(j == 1){
    j = 0;
    i += 1;
  } else{ j+= 1;}
}

//Funcao que realiza a captacao e envio de mensagens
void receiveMessage(){
  
  //Recebe a mensagem
  if (rf95.available()){
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)){
      if((buf[0] == '0' || buf[0] == numero_retransmissao-1 || numero_retransmissao == '1' && buf[0] == '9')){
        //Caso a mensagem seja captada normalmente
        Serial.println((char*) buf);
        Serial.print("---,+");
        if(numero_retransmissao < '9') buf[0]=numero_retransmissao;
        else buf[0]='1';
        //Retransmite a mensagem
        //uint8_t data[] = "1Mensagem recebida.";
        digitalWrite(led, HIGH);
        rf95.send(buf, len);
        rf95.waitPacketSent();
        
        
        Serial.print("---,");
        Serial.print(rf95.lastRssi(), DEC);
        Serial.println(";");
        digitalWrite(led, LOW);
        Serial.println((char*) buf);
        S += 1;
      }
    }
   else{
    //Caso a mensagem seja captada, mas ocorra um erro
     Serial.println("Falha ao receber a mensagem.");
     F += 1; 
    }
  }
}
