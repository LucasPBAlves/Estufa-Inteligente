/***********************************************************************
* FILENAME : Estufa_Funcao.ino DESIGN REF: FMCM00
*
* DESCRIPTION :
* Hardware functions integrated with bluetooth to make a smart GreenHouse.
*
* AUTHORS : Augusto Freitas Franco Gomes
*           Gabriel da Silva Gonçalves
*           Leonardo Piuzana Pizano 
*           Lucas Pimenta Brito Alves
*           Maurílio Rittershaussen Novaes
*           
* START DATE : 20 May 22
*
* CHANGES :
*
* REF NO VERSION DATE WHO DETAIL
* 27Jun22 JR Procedures works correctly 
*
***********************************************************************/


#include <SoftwareSerial.h> // Comandos Seriais Bluetooth
#include <DHT.h> //Carrega a biblioteca DHT - Sensor
#include <Servo.h>


#define DHTPIN A5 //Define a ligação ao pino de dados do sensor DHT11
#define DHTTYPE DHT11 //Define o tipo de sensor DHT utilizado
DHT dht(DHTPIN, DHTTYPE);

#define SERVO 3  // Porta Digital 3 - Servo Motor
Servo s; // Variável Servo
int pos; // Posição Servo

//Sensor de Umidade deo solo
const int pinoSensor = A0; //Sensor de umidade do solo
int valorSolo; //Variável que armazena a leitura do valor do solo
int analogSoloSeco = 1000; //Valor medido com o solo seco - NECESSITA TESTE
int analogSoloMolhado = 300; //Valor medido com o solo úmido -NESSECITA TESTE
int percSoloSeco = 0; //Menor percentual do solo seco (0%)
int percSoloMolhado = 100; //Maior percentual do solo seco (100%)

//Variáveis que armazenam os valores enviados pelo Bluetooth
int dadoBluetooth = 0;
int luzBluetooth = 3;
int aguaBluetooth = 5;
int portaBluetooth = 7;
int valorRecebido=0;
int tpLuz=0;
int intLuz=0;
int tpAgua=0;
int intAgua=0;
int tpPorta=0;
int intPorta=0;

void setup() {

  Serial.begin(9600);
  
  //Servo
  s.attach(SERVO);
  Serial.println("Você já pode se conectar à estufa!");
  
  
  dht.begin();// Começa o sensor DHT
  
  pinMode(10, OUTPUT); // Porta 10 - Rele da lampada
  pinMode(11, OUTPUT); // Porta 11 - Bomba Dagua
  
  s.write(0); // Inicia motor posição zero
}

//Função Para Timer da Luz
void LigaTimerLuz(int dadoBluetooth,int tpLuz, int intLuz){
  intLuz= intLuz*600000;//Tempo em minuto
  tpLuz= tpLuz*600000;//Tempo em minuto
  
  while(dadoBluetooth!=0 || tpLuz != 0 || intLuz != 0){
    digitalWrite(10, LOW);
    delay(intLuz);
    digitalWrite(10, HIGH);
    delay(tpLuz);
  }
}

//Função Para Timer da Agua
void LigaTimerAgua(int dadoBluetooth,int tpAgua, int intAgua){
  intAgua= intAgua*600000;//Tempo em minuto
  tpLuz= tpAgua*600000;//Tempo em minuto
  
  while(dadoBluetooth!=0 || tpAgua != 0 || intAgua != 0){
    digitalWrite(11, LOW);
    delay(intAgua);
    digitalWrite(11, HIGH);
    delay(tpAgua);
  }
}

//Função Para Timer da Porta
void LigaTimerPorta(int dadoBluetooth,int tpPorta, int intPorta){
  intPorta= intPorta*60000;//Tempo em minuto
  tpPorta= tpPorta*60000;//Tempo em minuto
  
  while(dadoBluetooth!=0 || tpPorta != 0 || intPorta != 0){
    s.write(0);
    delay(intPorta);
    s.write(70);
    delay(tpPorta);
  }
}


void loop() {

  //Variaveis do sensor DHT11
  float h = dht.readHumidity(); //Le o valor da umidade
  float t = dht.readTemperature(); //Le o valor da temperatura
  
  
  //Variaveis do Sensor LM393 - Solo
  valorSolo = constrain(analogRead(pinoSensor),analogSoloMolhado,analogSoloSeco); //Mantém -valorSolo- dentro do intervalo (ENTRE analogSoloMolhado E analogSoloSeco)
  valorSolo = map(valorSolo,analogSoloMolhado,analogSoloSeco,percSoloMolhado,percSoloSeco); //Executa a função "map" de acordo com os parâmetros passados
  
  if (isnan(t) || isnan(h)){
    Serial.println("Failed to read from DHT");
  }
  else{
    Serial.print("TEMP|");
    Serial.println(t);//imprime temperatura do ar
    Serial.print("|");
    delay(300);
    Serial.print("UMD|");
    Serial.println(valorSolo);//imprime umidade do solo
    Serial.print("|");
  }
  
  //Pareamento e armazenamento de informação Bluetooth
  if(Serial.read()>-1){
    valorRecebido=Serial.read();
    Serial.print("SERIAL READ");
    Serial.println(valorRecebido);
  }
  //Read Função Automática ou Manual
  if(valorRecebido ==0){
    dadoBluetooth = 0; 
  }
  if(valorRecebido == 1){
    dadoBluetooth = 1;
  }
  //Read Luz Ligada ou Desligada
  if(valorRecebido == 2){
    luzBluetooth = 2;
  }
  if(valorRecebido == 3){
    luzBluetooth = 3;
  }
  //Read Agua Ligada ou Desligada
  if(valorRecebido == 4){
    aguaBluetooth = 4;
  }
  if(valorRecebido == 5){
    aguaBluetooth = 5;
  }
  //Read Servo Ligado ou Desligado
  if(valorRecebido == 6){
    portaBluetooth = 6;
  }
  if(valorRecebido == 7){
    portaBluetooth = 7;
  }
  
  //Read Função manual de Luz - Tempo de Funcionamento
  if(valorRecebido == 8){
     tpLuz = Serial.read();
     Serial.print("TP READ");//Debug
     Serial.println(tpLuz);
  }
  //Read Função manual de Luz - Intervalo de Tempo
  if(valorRecebido == 9){
    intLuz=Serial.read();
    Serial.print("INT READ");//Debug
    Serial.println(intLuz);
  }
  //Read Função manual de Agua - Tempo de Funcionamento
  if(valorRecebido == 10){
    tpAgua=Serial.read();
  }
  //Read Função manual de Agua - Intervalo de Tempo
  if(valorRecebido == 11){
    intAgua=Serial.read();
  }
  //Read Função manual do Servo - Tempo de Funcionamento
  if(valorRecebido == 12){
    tpPorta=Serial.read();
  }
  //Read Função manual do Servo - Intervalo de Tempo
  if(valorRecebido == 13){
    intPorta=Serial.read();
  }
  
  
  //Modo Manual
  if(dadoBluetooth == 1){
    if(luzBluetooth==2){
      digitalWrite(10,HIGH);
    }
    else{
      digitalWrite(10,LOW);
    }
    if(aguaBluetooth==4){
      digitalWrite(11,LOW);
    }
    else{
      digitalWrite(11,HIGH);
    }
    if(portaBluetooth==6){
      s.write(pos=70);
    }
    else{
      if(pos==70){
        s.write(pos=0);
      }
    }
  }

  //Chama os procedimentos para ativar os timers
  LigaTimerLuz (dadoBluetooth, tpLuz, intLuz);
  LigaTimerAgua (dadoBluetooth, tpAgua, intAgua);
  LigaTimerPorta (dadoBluetooth, tpPorta, intPorta);
  
  
  if(dadoBluetooth == 0){
  
    if (t<24){ //Liga a Lampada para aquecimento
      digitalWrite(10, HIGH); //Liga a Lampada
    }
    else{ //Desliga a Lampada para aquecimento
      digitalWrite(10, LOW); //Desliga a Lampada
    }
    
    if (valorSolo>=40 && valorSolo){ //Liga a Bomba d'água
      digitalWrite(11, HIGH);
    }
    else{ //Desliga a Bomba d'agua
      digitalWrite(11, LOW); 
    }
    
    if (t>28){ //Abre a porta atraves do Servo
      s.write(pos=70);
      delay(15);
    }
    else{ //Fecha a porta atraves do servo
      if(pos == 70){
        s.write(pos=0);
      }
    } 
  }
  
  delay(500);
}
