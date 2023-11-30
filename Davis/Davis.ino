#include <EEPROM.h>

// Definiciones de pines
#define PinPot A0      // Pin del potenciómetro del veleta
#define PinCal A1      // Pin del botón de calibración
#define PinSensorRPM 2 // Pin del sensor del anemómetro

// Constantes para la conversión de RPM a m/s
const float RPMaMS[] = {0.0, 0.3333333, 0.3194444, 0.3055556, 0.2777778, 0.2777778};

// Variables
volatile int direction = 0;  // Dirección del viento (0-359)
int ValorPot = 0;            // Valor del potenciómetro del veleta
int CorreccionDir = 0;       // Corrección de dirección (-360 a +360)
volatile unsigned long ContadorRPM = 0;  // Contador de pulsos del anemómetro en la rutina de interrupción
volatile unsigned long TiempoContacto = 0;  // Temporizador para evitar rebotes en la interrupción
float speed = 0.0; // Velocidad del viento en m/s

void calibrar();
void rpm();

////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.println("Listo");
  delay(2000);
  
  // CALIBRAR si el botón está presionado al iniciar
  if (analogRead(PinCal) < 512) {
    calibrar();
  }
  // sino, recuperar los valores CAL de la EEPROM
  else {
    CorreccionDir = EEPROM.read(1) + EEPROM.read(2);
  }
  
  pinMode(PinSensorRPM, INPUT);
  attachInterrupt(digitalPinToInterrupt(PinSensorRPM), rpm, FALLING);
}

void loop() {
  direction = getDireccion();
  speed = getVelocidad();
  mostrar(speed, direction);
}

// Función para obtener la dirección del viento
int getDireccion() {
  ValorPot = analogRead(PinPot);
  direction= map(ValorPot, 0, 1023, 0, 359); 
  direction = (direction + CorreccionDir + 3) % 360; // Corregir offset y precisión de 5°
  return direction;
}

// Función para mostrar la dirección del viento en el puerto serie
void mostrarDireccion(int direccion) {
  Serial.print("Dirección: ");
  // Imprimir el valor de la dirección con ceros al frente para una precisión de 5°
  Serial.print(direccion < 100 ? "0" : "");
  Serial.print(direccion < 10 ? "0" : "");
  Serial.println(direccion, DEC);
}

// Función para obtener la velocidad del viento
float getVelocidad() {
  ContadorRPM = 0; // Reiniciar el contador de pulsos antes de cada medición
  sei();           // Habilitar interrupciones
  delay(500);      // Esperar 0.5 segundos para promediar
  cli();           // Deshabilitar interrupciones

  // Convertir a m/s
  int indice = min((ContadorRPM - 1) / 24, 4);
  speed = ContadorRPM * RPMaMS[indice];
  
  return speed;
}

// Función para mostrar la velocidad del viento en el puerto serie
void mostrarVelocidad(float velocidad) {
  Serial.print("Velocidad: ");
  // Imprimir la velocidad en m/s con espacio al frente para alineación
  Serial.print(velocidad < 10 ? " " : "");
  Serial.println(velocidad, 1); // 1 decimal para m/s
}

// Función de interrupción para medir RPM  
void rpm() {
  if ((millis() - TiempoContacto) > 15) { // Debounce del contacto del REED. Con 15 ms, se pueden medir velocidades de más de 150 km/h
    ContadorRPM++; 
    TiempoContacto = millis();
  }
}

// Función para calibrar el veleta
void calibrar() {
  Serial.print("Mantén presionado para calibrar!"); 
  delay(2000); // Esperar 2 segundos
  
  if (analogRead(PinCal) > 512) {
    setup(); // No se requiere CAL realmente... ¡abortar!
  }

  Serial.print("Ahora calibrando...    "); 
  delay(1000); // Esperar 1 segundo
  
  ValorPot = analogRead(PinPot); // Leer el valor del potenciómetro
  CorreccionDir = map(ValorPot, 0, 1023, 359, 0);

  Serial.print("Valor CAL = "); 
  Serial.print(CorreccionDir, DEC); 
  Serial.print("            ");  
  delay(2000); // Esperar 2 segundos  
  
  // Guardar los valores de calibración en la EEPROM
  EEPROM.write(1, CorreccionDir / 255);
  EEPROM.write(2, CorreccionDir % 255);
  
  // Esperar hasta que se suelte el botón de calibración
  while (analogRead(PinCal) < 512) {
    Serial.print("CAL OK - ¡Suelta!   ");
    delay(1000);
  }

  Serial.print("Ahora reiniciando...    ");   
  delay(1000);     

  setup(); // Reiniciar el dispositivo después de la calibración
}

// Función para mostrar la velocidad y dirección del viento
void mostrar(float velocidad, int direccion) {
  Serial.print("Velocidad: ");
  // Imprimir la velocidad en m/s con espacio al frente para alineación
  Serial.print(velocidad < 10 ? " " : "");
  Serial.print(velocidad, 1); // 1 decimal para m/s
  Serial.print("\t");

  Serial.print("Dirección: ");
  // Imprimir el valor de la dirección con ceros al frente para una precisión de 5°
  Serial.print(direccion < 100 ? "0" : "");
  Serial.print(direccion < 10 ? "0" : "");
  Serial.println(direccion, DEC);
}
