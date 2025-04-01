// CODIGO PARA MOTOR DE 2 TIEMPOS

//PINES PARA LOS POT LINEALES
const int potPin1 = 34; 
const int potPin2 = 34;
const int potPin3 = 34;
const int potPin4 = 34;
const int potPin5 = 34;

const int button = 35; // FINAL DE CARRERA

//LED'S RGB
const int ledAzul1 = 4; //LED1
const int ledRojo1 = 2;

const int ledAzul2 = 16; //LED2
const int ledRojo2 = 17;

const int motor = 23; // TALVES LA QUITEMOS


//VARIABLES AUXILIARES
unsigned long previousTime = 0;
int potValor = 0;
int ciclo = 0;
int periodo = 0;

const int frecuencia = 500;
const int resolucion = 8;
const int segundo = 2000;

void setup() { 
  Serial.begin(9600); // INICIALIZA EL SERIAL
  ledcAttach(motor, frecuencia, resolucion);
  ledcAttach(ledAzul1, frecuencia, resolucion);  
  ledcAttach(ledAzul2, frecuencia, resolucion);  
  pinMode(button, INPUT);
  pinMode(ledRojo1, OUTPUT);
  pinMode(ledRojo2, OUTPUT);
}

void loop() {
  unsigned long currentTime = millis();
  if((currentTime - previousTime) >= segundo){
      previousTime = currentTime;
      Serial.println(ciclo);
      ciclo = map(ciclo, 0, 10, 55, 255);
      ledcWrite(motor, ciclo);
      ciclo = 0;
  }

  potValor = analogRead(potPin);
  potValor = map(potValor, 0, 4095, 0, 255);
  

  if(digitalRead(button) == HIGH){
    digitalWrite(ledRojo, HIGH);
    ledcWrite(ledAzul, LOW);
  }else{
    ledcWrite(ledAzul, potValor);
    digitalWrite(ledRojo, LOW);
  }

  potValor = map(potValor, 0, 255, 0, 100);
  //Serial.println(potValor);
  if(periodo == 0){
    if((potValor >= 90) && (potValor <= 100))
    {
      periodo = 1;
    }
  }else
  {
    if((potValor >= 0) && (potValor <= 10))
    {
      periodo = 0;
      ciclo++;
    } 
  }

  delay(100);
}
