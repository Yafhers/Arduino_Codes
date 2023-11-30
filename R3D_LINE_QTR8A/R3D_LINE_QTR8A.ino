#include <QTRSensors.h>
QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

//Definicion de pines de los motores
#define izq1 9                                 //AIN1 Motor Izquierdo
#define izq2 10                                //AIN2 Motor Izquierdo
#define der1 6                                 //BIN1 Motor Derecho
#define der2 7                                 //BIN2 Motor Derecho
#define pwmA 11                                //PWM Motor Izquierdo    (D3 y D11 trabajan a 490 Hz en timer 2)
#define pwmB 3                                 //PWM Motor Derecho      (D3 y D11 trabajan a 490 Hz en timer 2)

//Definicion del boton
#define boton 2                             //Boton para calibrar los sensores

//Definicion de pines del QRT8A   
#define ledON 12
#define s0 A0
#define s1 A1
#define s2 A2
#define s3 A3
#define s4 A4
#define s5 A5
#define s6 A6
#define s7 A7

//Variables para la lectura y calibración del QTR8A
float position;
float ultima_posicion;

//Variables para el PID
float KP = 0.35;
float KD = 6.5;
float KI = 0.0025;
int vel = 100;
int velAdelante =   180;
int velAtras    =   -100;

//Variables de error 
int error = 0;
int errorDerivativo = 0;
int correccion = 0;
int last_error = 0;
int setpoint = 350;    //Posicion de la linea a seguir


void setup() {
    //Modificación de la frecuencia del PWM de los pines 3 y 11
    //TCCR2B = TCCR2B & B11111000 | B00000001;
    //set timer 2 divisor to     1 for PWM frequency of 31372.55 Hz

    //TCCR2B = TCCR2B & B11111000 | B00000010; 
    //set timer 2 divisor to     8 for PWM frequency of  3921.16 Hz

    TCCR2B = TCCR2B & B11111000 | B00000011;   
    //set timer 2 divisor to    32 for PWM frequency of   980.39 Hz

    //TCCR2B = TCCR2B & B11111000 | B00000100;   
    //set timer 2 divisor to    64 for PWM frequency of   490.20 Hz (Por Defecto)

    //TCCR2B = TCCR2B & B11111000 | B00000101;   
    //set timer 2 divisor to   128 for PWM frequency of   245.10 Hz

    //TCCR2B = TCCR2B & B11111000 | B00000110;   
    //set timer 2 divisor to   256 for PWM frequency of   122.55 Hz

    //TCCR2B = TCCR2B & B11111000 | B00000111;   
    //set timer 2 divisor to  1024 for PWM frequency of    30.64 Hz

    //Declaracion de pines del motor como salidas y del boton
    pinMode(izq1, OUTPUT);
    pinMode(izq2, OUTPUT);
    pinMode(der1, OUTPUT);
    pinMode(der2, OUTPUT);
    pinMode(pwmA, OUTPUT);
    pinMode(pwmB, OUTPUT);
    pinMode(boton, INPUT);

    //Monitor Serial
    Serial.begin(9600);
    Serial.println("Listo");

     //Configuración de los sensores
    qtr.setTypeAnalog(); //QTR8A
    qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7}, SensorCount);
    qtr.setEmitterPin(ledON); //Pin del LED_ON

    delay(500);
    // Si el led del Arduino esta encendido entonces el QTR se encuentra en modo calibración 
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 


    ////////////// CALIBRACIÓN /////////////
    //La calibración dura 10 segundos mientras el LED del arduino este encendido, durante esta fase debemos
    //poner los 8 sensores de reflectancia del color blanco al color negro todos al mismo tiempo durante
    //10 segundos. Con esto obtendremos cual valor representa al color mas claro y al mas oscuro.

    for (uint16_t i = 0; i < 400; i++) //Hacemos la calibración 400 veces, esto dura 10 segundos
    {
      qtr.calibrate();
    }
    digitalWrite(LED_BUILTIN, LOW); // Apaga el led del arduino para indicar que termina la calibración
  
    //Imprime los valores mínimos de calibración medidos cuando los emisores estaban encendidos
    Serial.begin(9600);
    for (uint8_t i = 0; i < SensorCount; i++)
    {
      Serial.print(qtr.calibrationOn.minimum[i]);
      Serial.print(' ');
    }
    Serial.println();

    //Imprime los valores máximos de calibración medidos cuando los emisores estaban encendidos
    for (uint8_t i = 0; i < SensorCount; i++)
    {
      Serial.print(qtr.calibrationOn.maximum[i]);
      Serial.print(' ');
    }
    Serial.println();
    Serial.println();
    delay(1000);
}

void loop() {

  int go  = digitalRead(boton);

  while(true){
    int go  = digitalRead(boton);
    posicion();
    frenos();
    PID();
    if(go==1){
      motores(-20,-20);
      break;
    }
  }

  while(true){
    motores(0,0);
  }
}

///////////Funcion controladora de motores///////////////////////
void motores(int izq, int der) {   
  //CONTROL DE MOTOR IZQUIERDO
  if(izq>=0){
    digitalWrite(izq1,HIGH);
    digitalWrite(izq2,LOW);
    analogWrite(pwmA,izq);
  }
  else{
    digitalWrite(izq1,LOW);
    digitalWrite(izq2,HIGH);
    analogWrite(pwmA,-izq);
  }

  //CONTROL DE MOTOR DERECHO
  if(der >= 0){
    digitalWrite(der1,LOW);
    digitalWrite(der2,HIGH);
    analogWrite(pwmB,der);
  }
  else{
    digitalWrite(der1,HIGH);
    digitalWrite(der2,LOW);
    analogWrite(pwmB,-der);
  }
}

///////////////////////////Funcio PID////////////////////////////
void PID(){
  error = position - setpoint;
  errorDerivativo = error - last_error;
  last_error = error;
  int correccion = (error*KP)+(errorDerivativo*KD);

  if (correccion<0){
    motores(vel,vel-correccion);
    Serial.print(vel);Serial.print(" ");Serial.println(vel-correccion);
  }

  else{
    motores(vel+correccion,vel);
    Serial.print(vel+correccion);Serial.print(" ");Serial.println(vel);
  }
}

/////////////////////////Funcio Frenos///////////////////////////
void frenos(){
  while(position==0 || position==700){
    if(position==0){
      motores(velAtras,velAdelante);
    }
    

    if(position==400){
      motores(velAdelante,velAtras);
    }

    posicion();
  }
}

void posicion(){
  // Lee los valores del sensor y obtiene una medida de la posición de la línea de 0 a 5000
 
  uint16_t position = qtr.readLineBlack(sensorValues); //(para una linea blanca, use readLineWhite())

  //Este programa imprime en el monitor serial valores del 0 al 1000:
  //-Reflectancia maxima (Blanco) -> 0 
  //-Reflectancia minima (Negro) -> 1000

  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println(position);
  
  delay(250);
}