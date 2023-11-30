#include <SoftwareSerial.h>

//Definicion de Pines (QTR8A - TB6612FNG - HC05)
#define s0 A0
#define s1 A1
#define s2 A2
#define s3 A3
#define s4 A4
#define s5 A5
#define s6 A6
#define s7 A7
#define ledQ 12
#define izq1 5  
#define izq2 4
#define der1 6
#define der2 7
#define pwmA 3
#define pwmB 11
#define stby 10
#define boton 2

SoftwareSerial HC05(0, 1); // RX, TX

//Variables de calibración
const int numero_sensores = 8;
int sensores[numero_sensores] = { s0, s1, s2, s3, s4, s5, s6, s7 };
int lectura_fondos[numero_sensores];  int lectura_lineas[numero_sensores];  int umbral[numero_sensores];
int analogico[numero_sensores];  int digital[numero_sensores];  //estos array guardan lecturas
float pos;  float ultima_posicion;

//PID
float KP = 0.42;  float KD = 10;  float KI = 0;

//Variables para el PID
int vel = 120;  int velAdelante = 130;  int velAtras = -80;
int error = 0;  int errorDerivativo = 0; int errorIntegral = 0; 
int error1=0; int error2=0; int error3=0; int error4=0; int error5=0; int error6=0; int last_error = 0;
int correccion = 0;
int setpoint = 350;

void setup() {
  //Timer 2 (pin 3 y 11) -> 980.39Hz
  TCCR2B = TCCR2B & B11111000 | B00000011;
  //Monitor Serial
  Serial.begin(9600);   Serial.println("Listo");
  //HC05
  HC05.begin(9600);
  //Declaracion de pines del motor como salidas y del boton
  pinMode(izq1, OUTPUT);
  pinMode(izq2, OUTPUT);
  pinMode(der1, OUTPUT);
  pinMode(der2, OUTPUT);
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(boton, INPUT);
  pinMode(stby, OUTPUT);  digitalWrite(stby, 1);
  //Declaración de pines del QTR8A
  pinMode(s0, INPUT);
  pinMode(s1, INPUT);
  pinMode(s2, INPUT);
  pinMode(s3, INPUT);
  pinMode(s4, INPUT);
  pinMode(s5, INPUT);
  pinMode(s6, INPUT);
  pinMode(s7, INPUT);
  pinMode(ledQ, OUTPUT); digitalWrite(ledQ, 1);
  //Etapa de calibración manual
  while (!digitalRead(boton));  fondos();     delay(500);
  while (!digitalRead(boton));  lineas();     delay(500);
  while (!digitalRead(boton));  promedios();  delay(500);
}

void loop() {
  int go = digitalRead(boton);

  while (true) {
    posicion();
    frenos();
    PID();
    if (go == 1) {
      motores(-20, -20);
      break;
    }

    if (HC05.available() > 0) {
    int receivedChar = HC05.read();

    if (receivedChar == '0') {
      KP += 0.1;
    }
    else if (receivedChar == '1') {
      KP -= 0.1;
    }
    else if (receivedChar == '2') {
      KD += 0.1;
    }
    else if (receivedChar == '3') {
      KD -= 0.1;
    }
    else if (receivedChar == '4') {
      KI += 0.01;
    }
    else if (receivedChar == '5') {
      KI -= 0.01;
    }
    else {
      // Ignorar 
    }
    }

    Serial.print("KP: "); Serial.print(KP);
    Serial.print(", KI: "); Serial.print(KI);
    Serial.print(", KD: "); Serial.println(KD);

  }

  while (true) {
    motores(0, 0);
  }
}

void motores(int izq, int der) {
  if (izq >= 0) {
    digitalWrite(izq1, HIGH);
    digitalWrite(izq2, LOW);
    analogWrite(pwmA, izq);
  } else {
    digitalWrite(izq1, LOW);
    digitalWrite(izq2, HIGH);
    analogWrite(pwmA, -izq);
  }

  if (der >= 0) {
    digitalWrite(der1, LOW);
    digitalWrite(der2, HIGH);
    analogWrite(pwmB, der);
  } else {
    digitalWrite(der1, HIGH);
    digitalWrite(der2, LOW);
    analogWrite(pwmB, -der);
  }
}

void fondos() {
  Serial.println("Lectura del fondo");  HC05.println("Lectura del fondo");

  for(int i = 0; i < numero_sensores; i++){
    lectura_fondos[i] = analogRead(sensores[i]);
    Serial.print(lectura_fondos[i]);  HC05.print(lectura_fondos[i]);
    Serial.print("\t"); HC05.print("\t");
  }

  Serial.println(""); HC05.println("");
}

void lineas() {
  Serial.println("Lectura de la línea");

  for (int i = 0; i < numero_sensores; i++) {
    lectura_lineas[i] = analogRead(sensores[i]);
    Serial.print(lectura_lineas[i]);  HC05.print(lectura_lineas[i]);
    Serial.print("\t"); HC05.print("\t");
  }

  Serial.println(""); HC05.println("");
}

void promedios() {
  Serial.println("Calculando umbral");

  for (int i = 0; i < numero_sensores; i++) {
    umbral[i] = (lectura_lineas[i] + lectura_fondos[i]) / 2;
    Serial.print(umbral[i]);  HC05.print(umbral[i]);  
    Serial.print("\t"); HC05.print("\t");
  }

  Serial.println(""); HC05.println("");
}

void posicion() {
  for(int i = 0; i < numero_sensores; i++){
    analogico[i] = analogRead(sensores[i]);

    if(analogico[i] >= umbral[i]){
      digital[i] = 1;}
    else{
      digital[i] = 0;}
  }

  int sumaPonderada = digital[0] * 0 + digital[1] * 100 + digital[2] * 200 + digital[3] * 300 + digital[4] * 400 + digital[5] * 500 + digital[6] * 600 + digital[7] * 700;
  int sumaTotal = digital[0] + digital[1] + digital[2] + digital[3] + digital[4] + digital[5] + digital[6] + digital[7];
  pos = sumaPonderada / sumaTotal;

  if (ultima_posicion <= 50 && pos == -1) pos = 0;
  if (ultima_posicion >= 650 && pos == -1) pos = 700;
  ultima_posicion = pos;

  //imprimeLecturasAnalogicas();
  imprimeLecturasDigitales();
  
}

void imprimeLecturasAnalogicas() {
  for (int i = 0; i < numero_sensores; i++) {
    Serial.print(analogico[i]); HC05.print(analogico[i]);
    Serial.print("\t"); HC05.print("\t");
  }

  Serial.print("\t"); HC05.print("\t");
  Serial.println(pos);  HC05.println(pos);
  Serial.println(""); HC05.println("");
}

void imprimeLecturasDigitales() {
  for (int i = 0; i < numero_sensores; i++) {
    Serial.print(digital[i]); HC05.print(digital[i]); 
    Serial.print("\t"); HC05.print("\t"); 
  }

  Serial.print("\t"); HC05.print("\t");
  Serial.println(pos);  HC05.println(pos);
  Serial.println(""); HC05.println("");
}

void PID() {
  error = pos - setpoint;
  errorDerivativo = error - last_error;
  errorIntegral = error1 + error2 + error3 + error4 + error5 + error6;

  last_error = error;
  error6 = error5;
  error5 = error4;
  error4 = error3;
  error3 = error2;
  error2 = error1;
  error1 = error;

  int correccion = (error * KP) + (errorDerivativo * KD) + (errorIntegral * KI);

  if (correccion < 0) {
    motores(vel, vel - correccion);
    Serial.print(vel);  Serial.print(" ");  Serial.println(vel - correccion);
  }

  else {
    motores(vel + correccion, vel);
    Serial.print(vel + correccion); Serial.print(" ");  Serial.println(vel);
  }
}

void frenos() {
  while (pos == 0 || pos == 700) {
    if (pos == 0) {
      motores(velAtras, velAdelante);
    }


    if (pos == 700) {
      motores(velAdelante, velAtras);
    }
    posicion();
  }
}