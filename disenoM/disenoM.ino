  // Pines de los potenciómetros
  const int potPins[4] = {27, 14, 12, 13};

  // Pines de los botones
  const int buttonPins[4] = {32, 33, 25, 26};

  // Pines de los LEDs RGB (catodo común)
  const int ledAzul[4] = {15, 16, 18, 3};
  const int ledVerde[4] = {2, 17, 19, 1};
  const int ledRojo[4] = {4, 5, 21, 22};
  

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

  // Configuración de PWM
  const int frecuencia = 500;
  const int resolucion = 8;

  void setup() {
    //Inicia el monitor serial
    Serial.begin(9600);

    // Configuración de pines
    for (int i = 0; i < 4; i++) {
      // Conectar los canales PWM a los pines GPIO usando ledcAttach
      ledcAttach(ledAzul[i], frecuencia, resolucion);
      ledcAttach(ledRojo[i], frecuencia, resolucion);
      ledcAttach(ledVerde[i], frecuencia, resolucion);
      
      // Configurar botones como entrada
      pinMode(buttonPins[i], INPUT);
    }   
  }

  void loop() {
    // Procesar cada pistón
    for (int i = 0; i < 4; i++) {
      procesarPiston(i);
    }
    delay(20); // Pequeño retraso para estabilidad
  }

  void procesarPiston(int indice) {
    // Leer valor del potenciómetro y normalizarlo
    int valorPot = analogRead(potPins[indice]);
    int valorPotMapeado = map(valorPot, 0, 4095, 0, 255);
    
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
        prenderLed(indice, 0, 0, 50); // Verde tenue
        
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