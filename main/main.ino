// Bibioteca do sensor de tempCorperatura
#include <Wire.h>
#include <Adafruit_MLX90614.h>                                         
// Bibioteca do sensor de distancia                      
#include <Ultrasonic.h>                                  
///////////////////////////////////////////////////////////

// Definindo pinos do sensor de distancia /////////////////
#define PINOTRIGGER 13
#define PINOECHO 12
///////////////////////////////////////////////////////////

// Definindo pinos do relay ///////////////////////////////
#define RELAY_VERMELHO 26                                   
#define RELAY_VERDE 25                                    
///////////////////////////////////////////////////////////

// Define constantes //////////////////////////////////////
#define MINDIST 0                                       
#define MAXDIST 10                                     
#define MINTEMP 30                                    
#define MAXTEMP 37                                 
#define MINNORMALTEMP 30                                    
#define MAXNORMALTEMP 60
#define TOTALREADTEMP 40                                 
///////////////////////////////////////////////////////////

// Variaveis obtidas na leitura ///////////////////////////
float dist, tempCorp, tempAmb, sumTemp, medTemp;                     
float readTemp[TOTALREADTEMP];                          
int loopReadTemp;                              
///////////////////////////////////////////////////////////

// Cria objeto do sensor de distancia /////////////////////
Ultrasonic ultrasonic(PINOTRIGGER, PINOECHO);            
///////////////////////////////////////////////////////////

// Cria objeto do sensor de tempCorperatura e umidade /////////
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
///////////////////////////////////////////////////////////

// Ler distância //////////////////////////////////////////
float lerDistancia () {
  long microsec = ultrasonic.timing();
  return ultrasonic.convert(microsec, Ultrasonic::CM);
}
//////////////////////////////////////////////////////////

// Ler temperatura ///////////////////////////////////
float lerTemp () {
  return mlx.readObjectTempC();
}
//////////////////////////////////////////////////////////

// Print distância lida ///////////////////////////////
void printDist () {
  Serial.print("Distancia em cm: ");
  Serial.println(dist);
}
//////////////////////////////////////////////////////////

// Print tempCorperatura lida ///////////////////////////////
void printTemp () {
  Serial.print("Temperatura: ");
  Serial.print(tempCorp);
  Serial.println(" *C");
}
//////////////////////////////////////////////////////////

// Ordena vetor temperatura /////////////////////////////
void ordenar () {
  int count, indice, i_maior, auxiliar;
  for (count = TOTALREADTEMP - 1; count > 0; count--) {
    for (indice = 1, i_maior = 0; indice < TOTALREADTEMP; indice++) {
      if(readTemp[indice] < readTemp[i_maior]) {
        i_maior = indice;
      }
    }
    auxiliar = readTemp[i_maior];
    readTemp[i_maior] = readTemp[indice];
    readTemp[indice] = auxiliar;
  }
}
//////////////////////////////////////////////////////////

// função que le x temperaturas para um vetor
//////////////////////////////////////////////////////////
void lerVet () {
  for(int i = 0; i < TOTALREADTEMP; i++){
      readTemp[i] = lerTemp();
      Serial.print("Temperatura n");
      Serial.print(i+1);
      Serial.print(": ");
      Serial.println(readTemp[i]);
      delay(50);
    }
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// função que calcula media somente com os valores dentro da faixa
// de valores aceitaveis /////////////////////////////////////////
//////////////////////////////////////////////////////////////////
float calcMedia () {
  int i, j, k;
  k = 0;
  sumTemp = 0;
  int equilibrio = floor((TOTALREADTEMP/3) * 2);
  for(i = j = TOTALREADTEMP - equilibrio; i < equilibrio; i++){
    if (MINNORMALTEMP < readTemp[i] && readTemp[i] < MAXNORMALTEMP){
      sumTemp += readTemp[i];
      j++;
      k++;
    }
  }
  if (!k) return 0;
  
  return sumTemp / k;   
}


// Liga o led verde //////////////////////////////////////
void onGreenLed () {
  digitalWrite(RELAY_VERMELHO, HIGH);
}
//////////////////////////////////////////////////////////

// Liga o led vermelho ///////////////////////////////////
void onRedLed () {
  digitalWrite(RELAY_VERDE, HIGH);
}
//////////////////////////////////////////////////////////

// Desliga o led verde ///////////////////////////////////
void offGreenLed () {
  digitalWrite(RELAY_VERMELHO, LOW);
}
//////////////////////////////////////////////////////////

// Desliga o led vermelho ///////////////////////////////
void offRedLed () {
  digitalWrite(RELAY_VERDE, LOW);
}
//////////////////////////////////////////////////////////

// funcao setup //////////////////////////////////////////
void setup() {
  // Definindo os modos dos pinos
  pinMode(RELAY_VERMELHO, OUTPUT);
  pinMode(RELAY_VERDE, OUTPUT);

  Serial.begin(9600);
  Serial.println();
  Serial.print("Iniciando dispositivos");
  mlx.begin();
  for(int i = 0; i < 3; i++){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  Serial.print("Iniciando leituras");
  for(int i = 0; i < 3; i++){
    delay(500);
    Serial.print(".");
  }
  Serial.println();

}
//////////////////////////////////////////////////////////

// funcao principal, o loop /////////////////////////////
void loop() {
  dist = lerDistancia();
  while ( !( MINDIST < dist && dist < MAXDIST ) ){
    dist = lerDistancia();
    Serial.println("Se aproxime ao sensor.");
  }
  delay(1000);
  for (loopReadTemp = 0; loopReadTemp < 5; loopReadTemp++) {

    Serial.println("-------------------------------");

    lerVet();
    
    Serial.println("-------------------------------");

    ordenar();

    medTemp = calcMedia();

    Serial.print("Temperatura media é: ");
    Serial.println(medTemp);
    
    Serial.println("-------------------------------");

    if ( MINTEMP < medTemp && medTemp < MAXTEMP ){
      onGreenLed();
      Serial.println("Sua temperatura está correta. Pode continuar");
      delay(2000);
      break;
    }

    if ( MAXTEMP < medTemp ){
      onRedLed();
      Serial.println("Sua temperatura não está bem.");
      delay(2000);
      break;
    }
  }
  if (loopReadTemp == 5 || !medTemp) Serial.println("Sua medição falhou!");

  offRedLed();
  offGreenLed();
  delay(3000);
}
//////////////////////////////////////////////////////////
