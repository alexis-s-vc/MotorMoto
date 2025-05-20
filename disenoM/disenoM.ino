/* ******************************************************
* MOTOR.ino
*
* Author:  Miguel Nahuatlato, Rocio Romero, Alexis Vázquez
* License: MIT
*
* Simulador de Motor de 4 Tiempos ESP32.
*
* *******************************************************/
//LINK DE LA SIMULACION: https://wokwi.com/projects/430884430846177281

// Pines de los botones (modificados para DevKit V1)
const int buttonPins[4] = {23, 22, 21, 19};

// Pines de los potenciómetros (ADC compatible)
const int potPins[4] = {36, 39, 34, 35};

// Pines de los LEDs RGB (GPIO que pueden ser salidas)
const int ledRojo[4] = {1, 14, 12, 4};
const int ledVerde[4] = {26, 13, 16, 2};
const int ledAzul[4] = {27, 15, 17, 5};

// Configuración del motor DC
const int motorPin = 3;           // Pin PWM para control del motor DC
const int motorButtonPins[4] = {18, 32, 33, 25}; // Botones para control de velocidad
const int motorSpeedLevels[4] = {0, 128, 180, 255}; // Valores PWM: 0%, 10%, 50%, 100%


// Estados del ciclo
enum EstadoMotor {
  ESPERANDO_INICIO,  // Esperando presionar botón para iniciar ciclo
  ADMISION,          // Potenciómetro subiendo/bajando - LED azul
  COMPRESION,        // Esperando botón para chispa
  EXPLOSION,         // Potenciómetro subiendo/bajando - LED amarillo (rojo+verde)
  ESCAPE             // Esperando siguiente ciclo
};

EstadoMotor estadoPiston[4] = {ESPERANDO_INICIO, ESPERANDO_INICIO, ESPERANDO_INICIO, ESPERANDO_INICIO};

// Variables para seguimiento de cada pistón
int valorAnterior[4] = {0, 0, 0, 0};
bool subiendo[4] = {true, true, true, true};
bool cicloCompletado[4] = {false, false, false, false};

// Variables para el control del motor DC
int currentMotorSpeed = 0;
bool motorButtonStates[4] = {false, false, false, false};
bool motorButtonPrevStates[4] = {false, false, false, false};

// Configuración del PWM
const int frecuencia = 500;
const int motorFreq = 1000;     // Frecuencia PWM para el motor
const int resolucion = 8;

void setup() {
  //Inicia el monitor serial
  Serial.begin(9600);

  // Configuración de pines
  for (int i = 0; i < 4; i++) {
    // Conectar los canales PWM a los pines GPIO usando ledcAttach
    ledcAttach(ledAzul[i], frecuencia, resolucion);
    ledcAttach(ledRojo[i], frecuencia+i, resolucion);
    ledcAttach(ledVerde[i], frecuencia+i+i, resolucion);
    
    // Configurar botones como entrada
    pinMode(buttonPins[i], INPUT_PULLDOWN);
    
    // Configurar botones del motor como entrada
    pinMode(motorButtonPins[i], INPUT_PULLDOWN);
  }
    
  // Configurar el canal PWM para el motor
  ledcAttach(motorPin, motorFreq, resolucion);
  ledcWrite(motorPin, 0); // Iniciar motor apagado
}

void loop() {
  // Procesar cada pistón
  for (int i = 0; i < 4; i++) {
    procesarPiston(i);
  }
  
  // Procesar control del motor DC
  procesarControlMotor();
  
  delay(20); // Pequeño retraso para estabilidad
}

// Controlar el motor DC con botones
void procesarControlMotor() {
  // Leer estado de cada botón del motor
  for (int i = 0; i < 4; i++) {
    // Leer estado actual del botón
    bool currentState = digitalRead(motorButtonPins[i]) == HIGH;
    
    // Detectar cambio de estado (flanco ascendente)
    if (currentState && !motorButtonPrevStates[i]) {
      // Botón presionado - establecer velocidad correspondiente
      currentMotorSpeed = motorSpeedLevels[i];
      
      Serial.print("Motor velocidad cambiada a: ");
      Serial.print(currentMotorSpeed);
      ledcWrite(motorPin, currentMotorSpeed);
      Serial.print(" (");
      if (i == 0) Serial.println("0%)");
      else if (i == 1) Serial.println("10%)");
      else if (i == 2) Serial.println("50%)");
      else if (i == 3) Serial.println("100%)");
    }
    
    // Actualizar estado previo para la próxima iteración
    motorButtonPrevStates[i] = currentState;
  }
}

void procesarPiston(int indice) {
  // Leer valor del potenciómetro y normalizarlo
  int valorPot = analogRead(potPins[indice]);
  int valorPotMapeado = map(valorPot, 4095, 0, 0, 255);
  
  // Verificar pulsación de botón con debounce
  bool botonPresionado = false;
  if (digitalRead(buttonPins[indice]) == HIGH) {
    botonPresionado = true;
  }
  
  // Detectar cambio de dirección en potenciómetro
  if (valorPotMapeado > valorAnterior[indice] + 5) {
    subiendo[indice] = true;
  } else if (valorPotMapeado < valorAnterior[indice] - 5) {
    subiendo[indice] = false;
  }
  
  // Máquina de estados para el ciclo del motor
  switch (estadoPiston[indice]) {
    case ESPERANDO_INICIO:
      // Todos los LEDs apagados
      apagarLeds(indice);
      
      // Botón inicia ciclo de admisión
      if (botonPresionado) {
        estadoPiston[indice] = ADMISION;
        cicloCompletado[indice] = false;
      }
      break;
      
    case ADMISION:
      // LED azul sigue el potenciómetro
      prenderLed(indice, valorPotMapeado, 0, 0); // Azul
      
      // Si completó un ciclo (subió y bajó), avanza a compresión
      if (subiendo[indice] == false && valorPotMapeado < 20 && valorAnterior[indice] > 20) {
        cicloCompletado[indice] = true;
        estadoPiston[indice] = COMPRESION;
        apagarLeds(indice);
      }
      break;
      
    case COMPRESION:
      // Esperar botón para chispa
      if (botonPresionado) {
        // Simular chispa momentánea (rojo intenso)
        prenderLed(indice, 0, 255, 0); // Rojo brillante
        estadoPiston[indice] = EXPLOSION;
      }
      break;
      
    case EXPLOSION:
      // LED amarillo (rojo+verde) sigue el potenciómetro
      prenderLed(indice, 0, valorPotMapeado, valorPotMapeado); // Amarillo
      
      // Si completó un ciclo (subió y bajó), avanza a escape
      if (subiendo[indice] == false && valorPotMapeado < 20 && valorAnterior[indice] > 20) {
        estadoPiston[indice] = ESCAPE;
        apagarLeds(indice);
      }
      break;
      
    case ESCAPE:
      // Verde tenue indicando fase de escape
      prenderLed(indice, 0, 0, 20); // Verde tenue
      
      // Botón reinicia el ciclo a ADMISION
      if (botonPresionado) {
        estadoPiston[indice] = ADMISION;
      }
      break;
  }
  
  // Actualizar valor anterior para la próxima iteración
  valorAnterior[indice] = valorPotMapeado;
  
  // Debug en serial
  if (indice == 0) { // Solo para el primer pistón, para no saturar
    Serial.print("Pistón ");
    Serial.print(indice);
    Serial.print(": Estado=");
    Serial.print(estadoPiston[indice]);
    Serial.print(", Pot=");
    Serial.print(valorPotMapeado);
    Serial.print(", Subiendo=");
    Serial.println(subiendo[indice]);
  }
}

void prenderLed(int indice, int valorAzul, int valorRojo, int valorVerde) {
  // Catodo común: 255 = máximo brillo , 0 = apagado
  ledcWrite(ledAzul[indice], valorAzul);
  ledcWrite(ledRojo[indice], valorRojo);
  ledcWrite(ledVerde[indice], valorVerde);
}

void apagarLeds(int indice) {
  // ponemos en bajo para apagar
  ledcWrite(ledAzul[indice], 0);   // Azul apagado
  ledcWrite(ledRojo[indice], 0);   // Rojo apagado
  ledcWrite(ledVerde[indice], 0);  // Verde apagado
}
