#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>

// The serial connection to the GPS module
SoftwareSerial ss(4,5);
TinyGPSPlus gps;
float latitude , longitude;

void setup()
{
  Serial.begin(9600);
  ss.begin(9600);
}

void loop()
{
  while (ss.available() > 0)
  {
    if (gps.encode(ss.read()))
    {
      if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        Serial.print("Latitud:");
        Serial.println(String(latitude , 6));
        longitude = gps.location.lng();
        Serial.print("Longitud:");
        Serial.println(String(longitude , 6));
      }
    }
  }
}

