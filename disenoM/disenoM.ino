// CODIGO PARA MOTOR DE 2 TIEMPOS


const int potPin = 34;
const int button = 35;
const int ledAzul = 4;
const int ledRojo = 2;
const int motor = 23;


unsigned long previousTime = 0;
int potValor = 0;
int ciclo = 0;
int periodo = 0;

const int frecuencia = 500;
const int resolucion = 8;
const int segundo = 2000;

void setup() {
  Serial.begin(9600);
  ledcAttach(motor, frecuencia, resolucion);
  ledcAttach(ledAzul, frecuencia, resolucion);  
  pinMode(button, INPUT);
  pinMode(ledRojo, OUTPUT);

  delay(2000);
  ledcWrite(motor, 200);
  delay(1000);
  ledcWrite(motor, 0);
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
