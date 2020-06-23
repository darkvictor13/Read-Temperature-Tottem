// Biblioteca do sensor de tempCorperatura
#include <Wire.h>
#include <Adafruit_MLX90614.h>                                         
// Biblioteca do sensor de distancia                      
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
#define MINDIST 5                                      
#define MAXDIST 20                                    
#define MINTEMP 30                                    
#define MAXTEMP 37                                 
#define MINNORMALTEMP 34                                   
#define MAXNORMALTEMP 40
#define TOTALREADTEMP 50
#define FACTOR_EMICTION 1.132                            
///////////////////////////////////////////////////////////

// Variaveis obtidas na leitura ///////////////////////////
float dist, tempCorp, sumTemp, medTemp;                     
float readTemp[TOTALREADTEMP];                          
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

// funcao que fica procurando um objeto ate achar ///////
void achaPessoa () {
  dist = lerDistancia();
  while ( !( MINDIST < dist && dist < MAXDIST ) ) {
    dist = lerDistancia();
    Serial.println("Se aproxime ao sensor.");
  }
  return;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// função que le x temperaturas para um vetor
//////////////////////////////////////////////////////////
void lerVet () {
  for(int i = 0; i < TOTALREADTEMP; i++) {
      readTemp[i] = lerTemp() * FACTOR_EMICTION;
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
  int i, k;
  for(i = sumTemp = k = 0; i < TOTALREADTEMP; i++) {
    if (MINNORMALTEMP < readTemp[i] && readTemp[i] < MAXNORMALTEMP) {
      sumTemp += readTemp[i];
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
  for(int i = 0; i < 3; i++) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  Serial.print("Iniciando leituras");
  for(int i = 0; i < 3; i++) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

}
//////////////////////////////////////////////////////////

// funcao principal, o loop /////////////////////////////
void loop() {
  achaPessoa();
  delay(500);

  Serial.println("-------------------------------");
  lerVet();
  Serial.println("-------------------------------");

  medTemp = calcMedia();

  while(!medTemp) {
    achaPessoa();
    lerVet();
    medTemp = calcMedia();
  }

  Serial.print("Temperatura media é: ");
  Serial.println(medTemp);
  Serial.println("-------------------------------");

  if ( MINTEMP < medTemp && medTemp < MAXTEMP ) {
    onGreenLed();
    Serial.println("Sua temperatura está correta. Pode continuar");
  }else {
    onRedLed();
    Serial.println("Sua temperatura não está bem.");
  }
  delay(2000);
  offRedLed();
  offGreenLed();
  delay(2000);
}
//////////////////////////////////////////////////////////
