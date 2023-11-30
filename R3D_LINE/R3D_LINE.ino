//Definicion de pines de los motores
#define izq1 5   //AIN1 Motor Izquierdo
#define izq2 4   //AIN2 Motor Izquierdo
#define der1 8   //BIN1 Motor Derecho
#define der2 7   //BIN2 Motor Derecho
#define pwmA 11  //PWM Motor Izquierdo    (D3 y D11 trabajan a 490 Hz en timer 2)
#define pwmB 3   //PWM Motor Derecho      (D3 y D11 trabajan a 490 Hz en timer 2)
#define stby 6

//Definicion del boton
#define boton 2  //Boton para calibrar los sensores
#define LED 13   //Led Interno del Arduino

//Definicion de pines del QRT8A
#define ledON 12
#define s0 A7
#define s1 A6
#define s2 A5
#define s3 A4
#define s4 A3
#define s5 A2
#define s6 A1
#define s7 A0

//Variables para la lectura y calibración del QTR8A
float pos;
float ultima_posicion;
const int numero_sensores = 8;
int sensores[numero_sensores] = { s0, s1, s2, s3, s4, s5, s6, s7 };
int lecturas[numero_sensores];        //array que guarda la lectura analógica pura de los sensores
int digital[numero_sensores];         //array que guarda la conversión de la lectura analógica a digital
int lectura_fondos[numero_sensores];  //array para la calibración de fondos
int lectura_lineas[numero_sensores];  //array para la calibración de lineas
int umbral[numero_sensores];          //array para guardar el promedio de lineas y fondos (umbral)

//Variables para el PID
float KP = 0.42;
float KD = 10;
float KI = 0;
int vel = 120;
int velAdelante = 130;
int velAtras = -80;

//Variables de error
int error = 0;
int errorDerivativo = 0;
int correccion = 0;
int last_error = 0;
int setpoint = 350;  //Posicion de la linea a seguir


//Definicion de variables para el PID
float KP = 0.35;
float KD = 6.5;
float KI = 0.0025;
int vel = 100;
int velAdelante =   180;
int velAtras    =   -100;

//Definicion de variables para la integral
int error1=0;
int error2=0;
int error3=0;
int error4=0;
int error5=0;
int error6=0;

//Definicion de variables de error 
int error=0;
int errorIntegral = 0;
int errorDerivativo = 0;
int correccion = 0;
int last_error = 0;
int setpoint = 200;    //Posicion de la linea a seguir

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
  pinMode(stby, OUTPUT);
  digitalWrite(stby, HIGH);

  //Declaración de pines del QTR8A
  pinMode(s0, INPUT);
  pinMode(s1, INPUT);
  pinMode(s2, INPUT);
  pinMode(s3, INPUT);
  pinMode(s4, INPUT);
  pinMode(s5, INPUT);
  pinMode(s6, INPUT);
  pinMode(s7, INPUT);
  pinMode(ledON, OUTPUT);
  digitalWrite(ledON, HIGH);

  //Monitor Serial
  Serial.begin(9600);
  Serial.println("Listo");

  //Calibración
  while (!digitalRead(boton))
    ;
  fondos();  //Presiono el boton sobre el fondo
  delay(500);
  while (!digitalRead(boton))
    ;
  lineas();  //Presiono el boton sobre la linea
  delay(500);
  while (!digitalRead(boton))
    ;
  promedios();  //Realiza un promedio de los valores obtenidos
  delay(500);
}

void loop() {
  int go = digitalRead(boton);

  while (true) {
    int go = digitalRead(boton);
    posicion();
    frenos();
    PID();
    if (go == 1) {
      motores(-20, -20);
      break;
    }
  }

  while (true) {
    motores(0, 0);
  }
}

//////Funcion para calcular la lectura del fondo///////////////
void fondos() {
  Serial.println("Leyendo fondos: ");

  for (int i = 0; i < numero_sensores; i++) {
    lectura_fondos[i] = analogRead(sensores[i]);
    Serial.print(lectura_fondos[i]);
    Serial.print("\t");
  }

  Serial.println("");
}

//////Funcion para calcular la lectura de las lineas///////////
void lineas() {
  Serial.println("Leyendo lineas: ");

  for (int i = 0; i < numero_sensores; i++) {
    lectura_lineas[i] = analogRead(sensores[i]);
    Serial.print(lectura_lineas[i]);
    Serial.print("\t");
  }

  Serial.println("");
}

//////Funcion para calcular el promedio umbral/////////////////
void promedios() {
  Serial.println("Leyendo umbrales (calculando promedios): ");

  for (int i = 0; i < numero_sensores; i++) {
    umbral[i] = (lectura_lineas[i] + lectura_fondos[i]) / 2;
    Serial.print(umbral[i]);
    Serial.print("\t");
  }

  Serial.println("");
}

///////////Funcion controladora de motores///////////////////////
void motores(int izq, int der) {
  //CONTROL DE MOTOR IZQUIERDO
  if (izq >= 0) {
    digitalWrite(izq1, HIGH);
    digitalWrite(izq2, LOW);
    analogWrite(pwmA, izq);
  } else {
    digitalWrite(izq1, LOW);
    digitalWrite(izq2, HIGH);
    analogWrite(pwmA, -izq);
  }

  //CONTROL DE MOTOR DERECHO
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

//////Funcion que imprime la posicion de la linea////////////////
void posicion() {
  for (int i = 0; i < numero_sensores; i++) {
    lecturas[i] = analogRead(sensores[i]);

    if (lecturas[i] >= umbral[i]) {
      digital[i] = 1;
    } else {
      digital[i] = 0;
    }
  }

  int sumaPonderada = digital[0] * 0 + digital[1] * 100 + digital[2] * 200 + digital[3] * 300 + digital[4] * 400 + digital[5] * 500 + digital[6] * 600 + digital[7] * 700;
  int sumaTotal = digital[0] + digital[1] + digital[2] + digital[3] + digital[4] + digital[5] + digital[6] + digital[7];
  pos = sumaPonderada / sumaTotal;

  if (ultima_posicion <= 50 && pos == -1)
    pos = 0;
  if (ultima_posicion > 650 && pos == -1)
    pos = 700;
  ultima_posicion = pos;

  //imprimeLecturasAnalog();
  imprimeLecturasDig();
}

/////////Funcion que imprime lecturas analogicas//////////////
void imprimeLecturasAnalog() {
  for (int i = 0; i < numero_sensores; i++) {
    Serial.print(lecturas[i]);
    Serial.print("\t");
  }

  Serial.print("\t");
  Serial.println(pos);
  Serial.println("");
}

////////Funcion que imprime lecturas digitales//////////////
void imprimeLecturasDig() {
  for (int i = 0; i < numero_sensores; i++) {
    Serial.print(digital[i]);
    Serial.print("\t");
  }

  Serial.print("\t");
  Serial.println(pos);
  Serial.println("");
}

///////////////////////////Funcio PID////////////////////////////
void PID(){
  error = position-setpoint;
  errorDerivativo = error-last_error;
  errorIntegral = error1 + error2 + error3 + error4 + error5 + error6 ;
  last_error = error;
  error6 = error5;
  error5 = error4;
  error4 = error3;
  error3 = error2;
  error2 = error1;
  error1 = error;
  int correccion = (error*KP)+(errorDerivativo*KD)+(errorIntegral*KI);

  //if(correccion>vel);
  //correccion=vel;
  //if(correccion<-vel);
  //correccion=-vel;

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