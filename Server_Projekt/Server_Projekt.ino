#include <SoftwareSerial.h>
#include "DumbServer.h"

SoftwareSerial esp_serial(3, 2);
EspServer esp_server;
/*Der PIR Sensor wird für den PIN 9 deklariert */
int sensor = 9;
/*Keine Bewegung erfasst */                
int Zustand = LOW;
/* Der Anfangswert des Sensors ist 0 */             
int Wert = 0;
                  
/*Der Analog Pin für den Photosensor ist A5 */
int licht = A5;
/*Den Intenger Lichtwert deklarieren , der den Lichtwert des Photosensors erfasst*/                
int lichtwert;
/*Die Boolean Variable sorgt dafür das  */                 
boolean einmal = true;
/*Der intenger a ist der Anfangswert des Photosensors*/         
int a = 0;                    

void setup()
{
  Serial.begin(9600);
  esp_serial.begin(9600);

 /*Um das WLAN Shield zu starten benötigt man die WLAN Adresse , das Wlan Passwort und die Portnummer , dadurch wird
  * der Server gestartet
  */
  Serial.println("Starting server...");                               
  esp_server.begin(&esp_serial, "AndroidAP", "Briefkasten123", 30303);      
  Serial.println("...server is running");                              
/* Die IP wird ausgegeben um das Arduino Programm mit dem Python Programm zu verknüpfen */
  char ip[16];                           
  esp_server.my_ip(ip, 16);                                         

  Serial.print("My ip: ");
  Serial.println(ip);
/* PIR Sensor als INPUT deklarieren */
  pinMode(sensor, INPUT);                                             
}
/* Der Lichtwert wird am AnalogPin ausgelesen und am Seriellen Monitor wiedergegeben.
 *  Wenn der Licht wert kleiner gleich 200 ist , wird dem Server der Wert übermittelt.Zudem
 *  wird die boolean Variable auf true gesetzt, sobald der Lichtwert größer 200 ist und die Boolean
 *  Variable einmal auf true war. Gibt es den Wert ebenfalls an den Server weiter und die Boolean Variable
 *  wird auf false gesetzt
 */
void loop(){
    lichtwert = analogRead(licht);                                    
    Serial.println(lichtwert);                                       

    if(lichtwert <= 200){                                             
      esp_server.println(a);                                          
      einmal = true;                                                  
    }                                                                 
    else if (lichtwert > 200 && einmal==true){                        
      a++;                                                            
      esp_server.println(a);
      einmal = false;
    }
/* Der Sensorwer wird auf dem Digitalen Pin abgelesen, wenn der Wert auf High ist verzgert der Sensor um 2 Sekunden,
 *  Der Zustand wird daraufhin auf LOW gesetzt und ein X im seriellen Monitor angezeigt , danach wird der Zustand
 *  auf HIGH gesetzt , dann gibt es ein Delay ,wenn der Zustand von beginn HIGH ist , zeigt der serielle Monitor ein
 *  Y an . Danach wird der Zustand auf LOW gesetzt 
 */
Wert = digitalRead(sensor);                                        // Sensor Wert auf dem Digtalen Pin ablesen
  if (Wert == HIGH)
  esp_server.println(Wert);
  {                                              // Wenn der Wert auf HIGH ist 
    delay(2000)                                                    //Verzögere um 2 Sekunden.
                                                                     
    if (Zustand == LOW) {                                          //Ist der Zustand auf LOW
      Serial.println("x");                                         // Dann soll ein X auf dem Seriellen Monitor angezeigt werden
      Zustand = HIGH;                                              // Den Status auf HIGH setzen
    }
  } 
  else {
      delay(3000);                                                  // 3 Sekunden verzögern 
      
      if (Zustand == HIGH)
      esp_server.println(Wert);
      {                                        // Ist der Zustand auf HIGH
        Serial.println("y");                                       // Dann soll ein X auf dem Seriellen Monitor angezeigt werden
        Zustand = LOW;                                             // Den Variablen Status auf LOW setzen
    } 
  }

/*Die While Schleife wird aktiv , wenn Python ein Command schickt */
  
  while(esp_server.available()){                                     
      String command= esp_server.readStringUntil('\n');             

      if(command == "on"){
        a = 0;
        esp_server.println(a);
      }
      else if(command == "off"){
            a = 0;
        esp_server.println(a);  
      }
  }
}


