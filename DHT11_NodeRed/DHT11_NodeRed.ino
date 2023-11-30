#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

//VARIABLES PARA LA RED WIFI Y EL SERVER MQTT
char *server = "192.168.1.8"; 
int port = 1883;
const char *ssid = "CHESTER";
const char *passwd = "Manchitas";

//VARIABLES USADAS EN LAS FUNCIONES
char serial_command = -1;
unsigned long previousMillis = 0;
unsigned long interval = 30000;

//CONFIGURAR WIFI Y SERVIDOR MQTT
WiFiClient wlancliente;
PubSubClient mqttClient(wlancliente);

//CONFIGURAR DHT11
DHT dht(26, DHT11);
int hum = 0;  int temp = 0;
char msg[16];

void setup() {
  dht.begin(); delay(2000);
  Serial.begin(115200);

  //INICIAR WIFI Y MQTT
  WiFi.begin(ssid,passwd);
  mqttClient.setServer(server,port);
  mqttClient.setCallback(mqttCallBack);

  //CONEXION AL WIFI
  Serial.print("Conectando a la red WIFI");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");  delay(100);
  }
  Serial.println();
  Serial.println("Conectado a la red wifi :3, se obtuvo la IP: ");
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);  WiFi.persistent(true);

  //CONEXION AL BROKER MQTT 
  if(mqttClient.connect("ESP-Client",NULL,NULL)) {
    Serial.println ("Conectado al MQTT Broker");
  } else {
    Serial.println("Conexion fallida al MQTT Broker");
    Serial.println (mqttClient.state());
    delay(200);}
 
  //SUSCRIPCION DE TOPICOS
  mqttClient.subscribe("canal");
}

void loop() {
  //COMPROBAR CONEXION
  printWiFiStatus();
  if(!mqttClient.connected()) {
    reconnect();}
  mqttClient.loop();

  //LEER EL DHT11
  delay(2000);
  temp = dht.readTemperature(); 
  hum = dht.readHumidity();
  imprimirValores();

  //CONVERTIMOS LOS DATOS A CHAR PARA SER LEIDOS POR EL PAYLOAD
  snprintf(msg,16,"%d,%d",temp,hum);
  mqttClient.publish("canal",msg);
  delay(1000);
  
}

void imprimirValores(){
  Serial.print("Temp: "); Serial.print(temp); Serial.print(" C ");
  Serial.print("Humidity: "); Serial.print(hum); Serial.println(" % ");
}

void mqttCallBack(char *topic, byte *payload, unsigned int length){
  Serial.print("Mensaje recibido en el topico: ");
  Serial.println(topic);
}

void printWiFiStatus(){
  //IMPRIME EL ESTADO CADA 30 SEGUNDOS
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >=interval){
    switch (WiFi.status()){
      case WL_NO_SSID_AVAIL: Serial.println("No se puede alcanzar el SSID configurado de Wifi");
        break;
      case WL_CONNECTED: Serial.println("Conexión Wifi establecida exitosamente");
        break;
      case WL_CONNECT_FAILED: Serial.println("Conexion Wifi fallida");
        break;
    }
    Serial.printf("Estado de conexion: %d\n", WiFi.status());
    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
    previousMillis = currentMillis;
  }
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectar al MQTT broker..... ");

    if (mqttClient.connect("ESP-Client",NULL,NULL)) {
      Serial.println("Conectado al MQTT Broker");

      //suscribirse a los temas.

       mqttClient.subscribe("canal");

     
    } else {
      Serial.print("Fallo, rc=");
      Serial.println(mqttClient.state());
      Serial.println("Intentando conectar cada 5 segundos");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    printWiFiStatus();
  }
}