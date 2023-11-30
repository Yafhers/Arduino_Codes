#include <QTRSensors.h>
QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

void setup()
{
  //Configuración de los sensores
  qtr.setTypeAnalog(); //QTR8A
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7}, SensorCount);
  qtr.setEmitterPin(6); //Pin del LED_ON

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

void loop()
{ 
  
}
