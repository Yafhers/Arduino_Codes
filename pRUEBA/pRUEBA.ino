#include<SoftwareSerial.h>

double kp = 0.0;
double ki = 0.0;
double kd = 0.0;

SoftwareSerial HC05(0, 1); // RX, TX

void setup() {
  Serial.begin(9600);
  HC05.begin(9600);
}

void loop() {
  if (HC05.available() > 0) {
    int receivedChar = HC05.read();

    if (receivedChar == '0') {
      kp += 0.1;
    }
    else if (receivedChar == '1') {
      kp -= 0.1;
    }
    else if (receivedChar == '2') {
      kd += 0.1;
    }
    else if (receivedChar == '3') {
      kd -= 0.1;
    }
    else if (receivedChar == '4') {
      ki += 0.01;
    }
    else if (receivedChar == '5') {
      ki -= 0.01;
    }
    else {
      // Ignorar 
    }

  }

  Serial.print("kp: "); Serial.print(kp);
  Serial.print(", ki: "); Serial.print(ki);
  Serial.print(", kd: "); Serial.println(kd);
}