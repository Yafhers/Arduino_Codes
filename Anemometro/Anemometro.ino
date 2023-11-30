/* Lectura del anemometro David Wind Vane  
=================================================================== 
DAVIS Vantage Pro & Vantage Pro 2    Wind Sensor (speed & direction)

Black =  Pulso del anemometro (velocidad), Conectar al pin digital D2, y use resistencias de 4k7 como pull up a + 5v.
         usa de 10 a 22nF capacitor del pin D2 a tierra para rebotar el switch rojo del anemometro
       
Red =    Tierra

Green =  Pin de la dirección del viento - conectar a A0.  Usa de 1 ... 10 µF / 16v capacitor entre A0 y Tierra

Yellow = + 5v (Referencia del potenciometro)

*/

// include EEPROM write - se requiere para memorizar la configuración de antena/banda
#include <EEPROM.h>
#include <math.h>

int Direction ; // Direcion del viento

#define PotPin (A0)    // Define el pin de entrada para la direccion del viento edl potenciometro
int PotValue = 0;      // variable para almacenar el valor proveniente del potenciómetro
int DirCorr = 0;       // Corrección de la dirección ( - 360  to + 360)

#define CalPin (A1)    // defina el pin de entrada para iniciar la calibración de dirección en el arranque. Pin a tierra para calibrar
byte DirCorrB1 = 0;    // 2 bytes of DirCorr
byte DirCorrB2 = 0;

volatile unsigned long RPMTops;  // RPM supera el contador en la rutina de interrupción                           
volatile unsigned long ContactTime;  // Temporizador para evitar el rebote de contactos en la rutina de interrupción                            

float RPM;       // RPM count
                                    
#define RPMsensor (2)      // La ubicación del pin del sensor del anemómetro.


float temp = (0);

////////////////////////////////////////////////////////////////////
void setup() { 

  Serial.begin(9600);
  Serial.println(" DAVIS Readout");
  delay (2000);
  
// ¡CALIBRAR si el botón se presionó al inicio!

  if ((analogRead(CalPin)<512)) calibrate();


// de lo contrario, recuperar los valores de CAL de la EEPROM

  DirCorrB1 = EEPROM.read(1);
  DirCorrB2 = EEPROM.read(2);    
  DirCorr = (DirCorrB1) + (DirCorrB2);

  Serial.print("Dir  "); 

  pinMode(RPMsensor, INPUT); 
  attachInterrupt(0, rpm, FALLING); 

  Serial.print("Spd   0km/h");
    
 } 

/////////////////////////////////////////////////////////////////////////////////

void loop() { 

// Wind Direction
  
  PotValue = analogRead(PotPin);     // Lee el valor del potenciometro
  Direction = map(PotValue, 0, 1023, 0, 359); 
  Direction = Direction + DirCorr + 3;   // Correct for offset & 5° precision

 convert:       // Convert to 360°  
  
  if (Direction < 0) {
    Direction = Direction + 360;
    goto convert;
  }
  
  if (Direction > 360) {
    Direction = Direction - 360;
    goto convert;
  }
    
  if (Direction == 360) Direction = 0;
      if (Direction < 100)   Serial.print("0");
      if (Direction < 10)    Serial.print("0");            
      Serial.print(((Direction/5)*5), DEC);    // 5° precision is enough to print te direction value                
 
   if ((Direction)<23) {
    Serial.println(" N");
    } 
   if ((Direction>22) && (Direction<68)) {
    Serial.println("NE");
    } 
   if ((Direction>67) && (Direction<113)) {
    Serial.println(" E");
    } 
   if ((Direction>112) && (Direction<158)) {
    Serial.println("SE");
    } 
   if ((Direction>157) && (Direction<203)) {
    Serial.println(" S");
    } 
    if ((Direction>202) && (Direction<247)) {
    Serial.println("SW");
    } 
    if ((Direction>246) && (Direction<292)) {
    Serial.println(" W");
    } 
    if ((Direction>291) && (Direction<337)) {
    Serial.println("NW");
    } 
    if ((Direction>336) && (Direction<=360)) {
    Serial.println(" N");
    } 
      
         
  // measure RPM
  
   RPMTops = 0;   //Set NbTops to 0 ready for calculations
   sei();         //Enables interrupts
   delay (500);  //Wait 3 seconds to average
   cli();         //Disable interrupts

 
 // convert to km/h
 

 if ((RPMTops >= 0) and (RPMTops <= 21)) RPM = RPMTops * 1.2;
 if ((RPMTops > 21) and (RPMTops <= 45)) RPM = RPMTops * 1.15;
 if ((RPMTops > 45) and (RPMTops <= 90)) RPM = RPMTops * 1.1;
 if ((RPMTops > 90) and (RPMTops <= 156)) RPM = RPMTops * 1.0;
 if ((RPMTops > 156) and (RPMTops <= 999)) RPM = RPMTops * 1.0;

     
 // print the speed 
  
  if (RPM < 100)   Serial.print(" ");
  if (RPM < 10)    Serial.print(" ");            
  Serial.print(int(RPM), DEC); 

}


//// This is the function that interrupt calls to measure  RPM  

 void rpm ()   { 

    if ((millis() - ContactTime) > 15 ) {  // debounce of REED contact. With 15ms speed more than 150 km/h can be measured
      RPMTops++; 
      ContactTime = millis();
    }
    
} 
//// end of RPM measure  
 
  
//// This is the function that calibrates the vane

 void calibrate () {
   
  Serial.print("Manten presionado para calibrar !"); 
  delay (2000);  //Wait 2 second
  if ((analogRead(CalPin)>512)) setup();  // CAL not really required ... abort !

  Serial.print("Calibrando ...    "); 
  delay (1000);  //Wait 1 second
  
  PotValue = analogRead(PotPin);     // read the value from the potmeter
  DirCorr = map(PotValue, 0, 1023, 359, 0);

  Serial.print("CAL value = "); 
  Serial.print(DirCorr, DEC); 
  Serial.print("            ");  
  delay (2000);  //Wait 2 seconds  
  
//  
  DirCorrB1 = DirCorr / 255;
  if (DirCorrB1 == 1){ 
  DirCorrB1 = 255;  
  DirCorrB2 = DirCorr - 255 ;
    }
  else {
  DirCorrB1 = DirCorr;  
  DirCorrB2 = 0;
    }   
//
//  DirCorrB1 = DirCorr;  
//  DirCorrB2 = 0;
  EEPROM.write (1, DirCorrB1);
  EEPROM.write (2, DirCorrB2);

 wait:  
  Serial.print("CAL OK - Release !   ");  
  if ((analogRead(CalPin)<512)) goto wait;
  
  Serial.print("Now rebooting...    ");   
  delay (2000);     

  setup (); 
}