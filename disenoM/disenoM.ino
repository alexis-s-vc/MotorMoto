// Pines de los potenciómetros
const int potPin1 = 34;
const int potPin2 = 35;
const int potPin3 = 32;
const int potPin4 = 33;

// Pines de los botones
const int button1 = 14;
const int button2 = 12;
const int button3 = 13;
const int button4 = 27;

// Pines de los LEDs RGB
const int ledAzul1 = 4;
const int ledRojo1 = 2;
const int ledAzul2 = 17;
const int ledRojo2 = 16;
const int ledAzul3 = 18;
const int ledRojo3 = 5;
const int ledAzul4 = 23;
const int ledRojo4 = 22;

// Configuración de PWM
const int frecuencia = 500;
const int resolucion = 8;

void setup() {
  Serial.begin(9600);

  // Asignación de PWM a LEDs
  ledcAttach(ledAzul1, frecuencia, resolucion);
  ledcAttach(ledAzul2, frecuencia, resolucion);
  ledcAttach(ledAzul3, frecuencia, resolucion);
  ledcAttach(ledAzul4, frecuencia, resolucion);
 

  pinMode(ledRojo1, OUTPUT);
  pinMode(ledRojo2, OUTPUT);
  pinMode(ledRojo3, OUTPUT);
  pinMode(ledRojo4, OUTPUT);

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
}

void loop() {
  controlarLed(potPin1, button1, ledAzul1, ledRojo1, 0);
  controlarLed(potPin2, button2, ledAzul2, ledRojo2, 1);
  controlarLed(potPin3, button3, ledAzul3, ledRojo3, 2);
  controlarLed(potPin4, button4, ledAzul4, ledRojo4, 2);
  delay(100);
}

void controlarLed(int potPin, int buttonPin, int ledAzul, int ledRojo, int canalPWM) {
  int potValor = analogRead(potPin);
  potValor = map(potValor, 0, 4095, 0, 255);

  if (digitalRead(buttonPin) == HIGH) {
    digitalWrite(ledRojo, HIGH);
    ledcWrite(ledAzul, 0);
  } else {
    digitalWrite(ledRojo, LOW);
    ledcWrite(ledAzul, potValor);
  }
}
