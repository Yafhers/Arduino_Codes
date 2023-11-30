/****************************************************
  Código para calibración y seteo de angulo inicial de servomotores 
  con driver PCA9685, servomotor conectado en canal 0  

  Creado por: Carlos Nina Ochoa   @carlosninaochoa
  10 Febrero 2023

  Conexión:
  PCA9685 ->  Arduino Nano
    GND -> GND
    OE  ->
    SCL ->  A5
    SDA ->  A4
    VCC ->  5V
    V+  ->  [Fuente externa]
****************************************************/
  
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVO_FREQ 50 // Valor típico para servos analogicos
uint8_t servonum = 0;

// Variables para la recepcion de valores por conexion serial
String mensaje = "";
String comando = "";
int valor;
bool flag = 0;
bool estexto;

// Parametros del servo y valores iniciales referenciales
int minimo = 150;   // 150 corresponde a la posición 0 grados
int maximo = 500;   // 500 corresponde a la posición 180 grados
int posicion = 90; // angulo al que se moverá el eje del servo, para el ensamble


void setup() {
  // inicio de PWM
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  
  delay(10);

  //Inicio de conexion serial
  Serial.begin(9600);
  Serial.setTimeout(50);
  delay(10);
}

void loop() {

// Recibir el mensaje introducido por el usuario
if (Serial.available()){
    mensaje = Serial.readStringUntil('\n');    
}

// toInt() Convierte el mensaje enviado por el usuario en un valor entero. Devuelve 0 si el mensaje no es numérico
if(mensaje.toInt() == 0){ //El mensaje es un cero o un texto
  if(mensaje == "0"){
    //el menaje es un numero igual a cero
    estexto = 0;  
  } else {
    // El mensaje es un texto
    estexto = 1;
  }
} else { //El mensaje es un numero distinto a cero
  estexto = 0;  
}

valor = mensaje.toInt(); 

// Solicita ingresar el comando para empezar
if( estexto == 1 && flag == 0 && mensaje == "" ){  
  // Verifica que el mensaje es una cadena (estexto == 1) y esta vacia (mensaje == "")
  // flag = 0 es para indicar si ya se ha solicitado el comando (1) o no (0)  
  Serial.print("\nCalibración de servos");
  Serial.print("\n=====================");
  Serial.print("\nIngrese uno de los comandos para iniciar la calibración del servo [min, max, pos, ver]:");
  comando = "";
  flag = 1;
}

// Se solicita introducir el valor para el comando elegido
if( mensaje == "min" && flag == 1 && estexto == 1 ){
  comando = "min";
  Serial.print("\nComandos [min, max, pos, ver] | Introducir el pulso minimo: ");
}
if( mensaje == "max" && flag == 1 && estexto == 1 ){
  comando = "max";
  Serial.print("\nComandos [min, max, pos, ver] | Introducir el pulso maximo: ");
}
if( mensaje == "pos" && flag == 1 && estexto == 1 ){
  comando = "pos";
  Serial.print("\nComandos [min, max, pos, ver] | Introducir la posicion angular: ");
}
if( mensaje == "ver" && flag == 1 && estexto == 1 ){
  Serial.print("\nComandos [min, max, pos, ver] | Datos ingresados: min="); Serial.print(minimo);
  Serial.print(",max="); Serial.print(maximo);
  Serial.print(",pos="); Serial.print(posicion);
}

//Se recibe el valor para el comando introducido
if( comando == "min" && estexto == 0){
  minimo = valor;
  pwm.setPWM(0, 0, minimo);
  Serial.print("\nComandos [min, max, pos, ver] | El pulso minimo es: ");Serial.print(minimo);
}
if( comando == "max" && estexto == 0){
  maximo = valor;
  pwm.setPWM(0, 0, maximo);
  Serial.print("\nComandos [min, max, pos, ver] | El pulso maximo es: ");Serial.print(maximo);
}
if( comando == "pos" && estexto == 0 ){ 
  posicion = valor;
  pwm.setPWM(0, 0, AnguloAPulso(posicion,minimo,maximo));
  Serial.print("\nComandos [min, max, pos, ver] | La posicion angular es: ");Serial.print(posicion);  
}

delay(500);

}

//Función que mapea los valores de angulo (0 a 180 grados) 
//hacia valores de pulso, entre el minimo y maximo definidos

int AnguloAPulso (int angulo, int minimo, int maximo) {
  int pulso = map(angulo, 0, 180, minimo, maximo);
  return pulso;     
}