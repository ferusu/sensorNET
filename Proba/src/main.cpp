#include <Arduino.h>
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
//#include <WiFiManager.h>

// Configuración de la WiFi generada
const char *ssid = "canguele";
const char *password = "password";
int incomingByte = 0; // almacenar el dato serie
void setup() {
  Serial.begin(9600);
  delay(10);
  
  WiFi.mode(WIFI_AP);
  while(!WiFi.softAP(ssid, password))
  {
   Serial.println(".");
    delay(100);
  }
  
  Serial.print("Iniciado AP ");
  Serial.println(ssid);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
  pinMode(D4, OUTPUT);
}
 
void loop() {
  digitalWrite(D4, HIGH);   // turn the LED on (HIGH is the voltage level)
if (Serial.available() > 0) {
// lee el byte de entrada:
incomingByte = Serial.read();
//lo vuelca a pantalla
Serial.print("He recibido: "); Serial.println(incomingByte,DEC);
} 
  }