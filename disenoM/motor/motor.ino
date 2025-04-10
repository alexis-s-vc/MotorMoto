const int motor = 22;
const int botonera[4] = {13,12,14,27};

const int frecuencia = 500;
const int resolucion = 8;
int PWM;

void setup() {
  Serial.begin(9600);
  ledcAttach(motor, frecuencia, resolucion);
  pinMode(botonera[0], INPUT);
  pinMode(botonera[1], INPUT);
  pinMode(botonera[2], INPUT);
  pinMode(botonera[3], INPUT);
  delay(200);
}

void motorPWM();

void loop() {
  motorPWM();
  
}

void motorPWM(){
  if(digitalRead(botonera[0]) == HIGH){
    Serial.println(0);
    PWM = 0;
  }else
  if(digitalRead(botonera[1]) == HIGH){
    Serial.println(33);
    PWM = 85;
  }else
  if(digitalRead(botonera[2]) == HIGH){
    ledcWrite(motor, 100);
    delay(20);
    Serial.println(66);
    PWM = 170;
  }else
  if(digitalRead(botonera[3]) == HIGH){
    ledcWrite(motor, 100);
    delay(20);
    Serial.println(100);
    PWM = 255;
  }
  ledcWrite(motor, PWM);
}