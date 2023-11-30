#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver rightBoard = Adafruit_PWMServoDriver(0x40); // Driver PCA Derecho
Adafruit_PWMServoDriver leftBoard = Adafruit_PWMServoDriver(0x41); //Driver PCA Izquierdo
#define SERVO_FREQ 60 // Analog servos run at ~50 Hz updates

/* 
 * Array que contiene la información de calibracion y posicion inicial de cada servo:
 * ServoData[NumeroServo][X] = (pulso_min, pulso_max, ang_min, ang_max, ang_inicial)
 * NumeroServo = 0,1,2,3...19 (numero identificador del servo)
 * x = 0(pulso_min), 1(pulso_max), 2(ang_min), 3(ang_max), 4(ang_inicial)
*/

int ServoData[20][5] = {
                            {125,  488,  70, 103,  90  },       //1R
                            {140,  485,  0,  180,  90  },       //2R
                            {180,  540,  64, 180,  180 },       //3R
                            {143,  495,  10, 180,  90  },       //4R
                            {125,  490,  90, 180,  90  },       //5R
                            {134,  486,  90, 150,  90  },       //6R
                            {145,  545,  0,  180,  45  },       //7R
                            {105,  490,  0,  180,  0   },       //8R
                            {185,  560,  45, 180,  180 },       //9R
                            {100,  480,  0,  90,   45 },        //10W
                            {100,  430,  77, 110,  90  },       //1L
                            {222,  568,  0,  180,  90 },        //2L
                            {190,  540,  0,  116,  0   },       //3L
                            {130,  490,  0,  170,  90 },        //4L
                            {203,  550,  0,  90,   90  },       //5L
                            {210,  550, 30,  90,   90  },       //6L  
                            {207,  565,  0,  180,  135 },       //7L   
                            {150,  525,  0,  180,  180 },       //8L
                            {180,  555,  0,  135,  0   },       //9L
                            {152,  515,  0,  90,   45  }        //10N
};

// Variables
int desfase = 200; // Tiempo de espera entre cambios de angulo en los servos (en milisegundos)

void setup() {

  Serial.begin(9600);
  
  rightBoard.begin();
  leftBoard.begin();
  rightBoard.setOscillatorFrequency(27000000);
  leftBoard.setOscillatorFrequency(27000000);
  rightBoard.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  leftBoard.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  
  delay(100);
  //poner los servos en su angulo inicial (robot de pie con los brazos abajo)
  ServosCero();
  delay(400);
  
}

void loop() {
  
Serial.println("Ingrese comando: ");
while (Serial.available() == 0) {}     //espera por el comando
String mensaje = Serial.readString();  //lee el string hasta que se agote el tiempo
mensaje.trim();

if (mensaje == "saludar") {
    Serial.println("\nHaré un saludo");
    // rutina de saludo al recibir por el puerto serial el mensaje "saludar"
        // Rutina de movimiento: "saludar"
        int saludo[][21] = {
          {90,   90,   180,   90,   90,   90,   45+135,   0,   180,   45,   90,   90,   0,   90,   90,   90,   135,   180,   0,   45,   800},
          {90,   90,   180,   90,   90,   90,   45+135,   90,   180-20,   45,   90,   90,   0,   90,   90,   90,   135+10,   180-40,   0,   45,   200},
          {90,   90,   180,   90,   90,   90,   45+135,   40,   180-70,   45,   90,   90,   0,   90,   90,   90,   135,   180-20,   0,   45,   200},
          {90,   90,   180,   90,   90,   90,   45+135,   90,   180-20,   45,   90,   90,   0,   90,   90,   90,   135+10,   180-40,   0,   45,   200},
          {90,   90,   180,   90,   90,   90,   45+135,   40,   180-70,   45,   90,   90,   0,   90,   90,   90,   135,   180-20,   0,   45,   200},
          {90,   90,   180,   90,   90,   90,   45+135,   90,   180-20,   45,   90,   90,   0,   90,   90,   90,   135+10,   180-40,   0,   45,   200},
          {90,   90,   180,   90,   90,   90,   45+135,   40,   180-70,   45,   90,   90,   0,   90,   90,   90,   135,   180-20,   0,   45,   200},
          };
        int filas2 = sizeof(saludo)/sizeof(saludo[0]); // Obtiene el numero de filas del array saludo[][]
        MoverServos(saludo, filas2); // Ejecuta la rutina de movimiento
        ServosCero(); // Retorna el robot a su posición inicial
        delay(1000);
  }

if (mensaje == "sentar") {
    Serial.println("\nMe sentaré");
    // rutina de sentadilla al recibir por el puerto serial el mensaje "sentar"
          // Rutina de movimiento: "Sentadillas"
          int sentar[][21] = {
            {90,   90+20,   180-45,   90-40,   90,   90,   45+135,   0,   180,   45,   90,   90-15,   0+45,   90+40,   90,   90,   135-135,   180,   0,   45,   2000},
            {90,   90,   180,   90,   90,   90,   45,   0,   180,   45,   90,   90,   0,   90,   90,   90,   135,   180,   0,   45,   2000}
            };
          int filas1 = sizeof(sentar)/sizeof(sentar[0]); // Obtiene el numero de filas del array sentar[][]
          MoverServos(sentar, filas1); // Ejecuta la rutina de movimiento
          ServosCero(); // Retorna el robot a su posición inicial
          delay(1000);
  }
if (mensaje == "patear") {
    Serial.println("\nHaré una patada");
    // Rutina de movimiento: "patear"
          //{1R,   2R,      3R,       4R,     5R,     6R,  7R,  8R,   9R,   10W,   1L,   2L,      3L,     4L,     5L,    6L,   7L,   8L,   9L,   10N,   tiempo}
          int patear[][21] = {
            {90,   90,   180-165,   90-50,  90+10,90+30,45+15,   0+10,   180,   45,   90,   90,   0,   90,   90,   90,   135-25,   180-20,   0,   45,   1000},
            {90,   90,   180,   90-50,   90,   90,   45+25,   0+20,   180,   45,   90,   90,   0,   90,   90,   90,   135-25,   180-10,   0,   45,   1000},
            {90+13, 90,   180,   90-50,   90,   90,   45,   0+20,   180,   45,   90,   90,   0,   90,   90,   90,  135-25,   180,   0,   45,   500},
            {90,   90,   180,   90-50,   90,   90,   45,   0,   180,   45,   90,   90,   0,   90,   90,   90,   135,   180,   0,   45,   500},
            {90,   90,   180,   90,   90,   90,   45,   0,   180,   45,   90,   90,   0,   90,   90,   90,   135,   180,   0,   45,   500},
            {90,   90,   180,   90,   90,   90,   45,   0,   180,   45,   90,   90,   0,   90,   90,   90,   135,   180,   0,   45,   500},
            };
          int filas = sizeof(patear)/sizeof(patear[0]); // Obtiene el numero de filas del array patear[][]
          MoverServos(patear, filas); // Ejecuta la rutina de movimiento
          ServosCero(); // Retorna el robot a su posición inicial
          delay(1000);     
  }
  delay(200); 
}

//Función que mapea los valores de angulo (0 a 180 grados) hacia valores de pulso, entre el minimo y maximo definidos
int AnguloAPulso (int numservo, int angulo) {
  int pulso = map(angulo, 0, 180, ServoData[numservo][0], ServoData[numservo][1]); 
  //ServoData[NumeroServo][X] = (pulso_min, pulso_max, ang_min, ang_max, ang_inicial)
  return pulso;  
}

// Función de posiciona todos los servomotores en su posición inicial 
void ServosCero() {          

      for(int i=0; i<20; i++){
        PosicionarServo(i,ServoData[i][4]);
        //Serial.print("\nServo "); Serial.print(i);Serial.print(": Angulo: "); Serial.print(ServoData[i][4]);
        delay(300);
      }
  delay(200);
}

//Función que desplaza el servo a una nueva posición
void PosicionarServo(float numservo, float ang) {

  if(numservo >= 0 && numservo < 10){
    //Driver derecho
    rightBoard.setPWM(15-numservo, 0, AnguloAPulso(numservo,ang)); // rightBoard.setPWM(canal,0,pulso)    
  } else if(numservo >= 10 && numservo < 20){
    //Driver izquierdo
    leftBoard.setPWM(numservo-10, 0, AnguloAPulso(numservo,ang)); // leftBoard.setPWM(canal,0,pulso)
  } else {
    //Serial.println("Error: numservo debe ser entero, entre 0 y 19.");
  }  
  //Serial.print("\nServo: ");Serial.print(numservo);Serial.print(" Angulo: ");Serial.print(ang);
}

// Función que realiza la rutina de movimiento 
void MoverServos(int mov[][21], int filas){
  
  for(int i=0; i < filas; i++){ // recorre todas las filas del array. Todas las etapas del movimiento
    int duracion = mov[i][20];
    int tiempo = 0;
    int cont = 1;

    while (tiempo < duracion)
    {
      for(int j=0; j < 20; j++){ // recorre las primeras 20 columnas, posicionando cada servo
        
        float angulo_inicial;        
        if(i==0){// Si es la primera fila, el angulo inicial es la posición cero del servo
          angulo_inicial = ServoData[j][4];
        } else { // Si es otra fila, el ángulo inicial es la posición de la fila anterior
          angulo_inicial = mov[i-1][j];
        }
        float angulo_final = mov[i][j];
        float angulo_despl = angulo_final - angulo_inicial;

        float paso = angulo_despl * desfase / duracion;
        float angulo = angulo_inicial+paso*cont;
               
        PosicionarServo(j,angulo);               

      }      
      cont = cont + 1;
      tiempo = tiempo + desfase;
      delay(desfase);
    }     
    
  }
  
}
