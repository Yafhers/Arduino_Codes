#include <SoftwareSerial.h>
#define motor1A 8  
#define motor1B 9
#define motor2A 6
#define motor2B 7

SoftwareSerial HC05(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);
  HC05.begin(9600);
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);
  motores(0,0);
}

void loop() {

  if (HC05.available() > 0) {
    int receivedChar = HC05.read();

    if (receivedChar == 'S') {
      motores(0,0);
    }
    else if (receivedChar == 'L') {
      motores(1,0);
    }
    else if (receivedChar == 'G') {
      motores(1,0);
    }
    else if (receivedChar == 'H') {
      motores(-1,0);
    }
    else if (receivedChar == 'R') {
      motores(0,1);
    }
    else if (receivedChar == 'I') {
      motores(0,1);
    }
    else if (receivedChar == 'J') {
      motores(0,-1);
    }
    else if (receivedChar == 'F') {
      motores(1,1);
    }
    else if (receivedChar == 'B') {
      motores(-1,-1);
    }
    else {
      // Ignorar 
    }
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