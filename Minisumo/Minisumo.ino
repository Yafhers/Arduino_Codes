//Definición de pines del motor
#define motor1A 8
#define motor1B 9
#define motor2A 7
#define motor2B 6
#define pwmA 10
#define pwmB 11

//Definición de pines del sensor infrarrojo
#define irSensor1 A1
#define irSensor2 A0

//Definición de pines del sensor ultrasónico HCSR04
#define triggerPin 4
#define echoPin 5

//Variables globales
int distancia;
int ir1;
int ir2;
int flag;

void setup() {
  Serial.begin(9600);

  //Configuración de los pines del motor como salidas
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);

  //Configuración de los pines del sensor infrarrojo como entradas
  pinMode(irSensor1, INPUT);
  pinMode(irSensor2, INPUT);

  //Configuración de los pines del sensor ultrasónico HCSR04 como entradas/salidas
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  //Lectura de la distancia del sensor ultrasónico HCSR04
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  distancia = pulseIn(echoPin, HIGH)/58;

  //Lectura de los valores de los sensores infrarrojos
  ir1 = analogRead(irSensor1);
  ir2 = analogRead(irSensor2);

  //Banderilla para el estado
  flag = 0;

  //Evaluación de las condiciones para el movimiento del robot
  if(ir1 < 400 && ir2 < 400){
    //Si ambos sensores infrarrojos detectan una línea blanca, el robot debe retroceder y girar a la derecha
    motores(-1, 1);
    delay(1000);
    motores(1, -1);
    flag = 1;
  }
  else if(distancia < 10){
    //Si el sensor ultrasónico detecta un obstáculo a menos de 10 cm, el robot debe retroceder y girar a la derecha
    motores(-1, 1);
    delay(1000);
    motores(1, -1);
    flag = 2;
  }
  else if(ir1 < 400){
    //Si el sensor infrarrojo izquierdo detecta una línea blanca, el robot debe girar a la izquierda
    motores(-1, 0);
    flag = 3;
  }
  else if(ir2 < 400){
    //Si el sensor infrarrojo derecho detecta una línea blanca, el robot debe girar a la derecha
    motores(0, -1);
    flag = 4;
  }
  else{
    //Si ninguno de los sensores detecta una línea blanca ni un obstáculo cercano, el robot debe avanzar hacia adelante
    motores(1, 1);
    flag = 0;
  }

  //Visualizacion de datos en consola
  Serial.print("Distancia: ");  Serial.print(distancia); Serial.print("   ");
  Serial.print("Sensor izquierdo: ");  Serial.print(ir1); Serial.print("   ");
  Serial.print("Sensor derecho: "); Serial.print(ir2); Serial.print("   ");
  Serial.print("Estado: "); 
  switch(flag){
    case 0: Serial.println("Sin obstaculos, voy a avanzar :D"); break;
    case 1: Serial.println("Linea blanca!, voy a retroceder y girar a la derecha"); break;
    case 2: Serial.println("Un oponente a menos de 10cm >:(, voy a retroceder y girar a la derecha :D"); break;
    case 3: Serial.println("Linea blanca por la izquierda!, voy a retroceder y girar a la izquierda"); break;
    case 4: Serial.println("Linea blanca por la derecha!, voy a retroceder y girar a la derecha"); break;
    default: Serial.println("Sin obstaculos, voy a avanzar :D"); break;
  }

}


void motores(int motor1_value, int motor2_value){
  if(motor1_value == 1){
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor1B, LOW);
  }
  else if(motor1_value == 0){
    digitalWrite(motor1A, LOW);
    digitalWrite(motor1B, LOW);
  }
  else if(motor1_value == -1){
    digitalWrite(motor1A, LOW);
    digitalWrite(motor1B, HIGH);
  }

  if(motor2_value == 1){
    digitalWrite(motor2A, HIGH);
    digitalWrite(motor2B, LOW);
  }
  else if(motor2_value == 0){
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, LOW);
  }
  else if(motor2_value == -1){
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, HIGH);
  }
}