#include <EEPROM.h>
#include "DHT.h"

#define LIGHTLEVELPIN A0
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

int lightLevel;
String UUID = "";

void setup() {
  Serial.begin(38400);
  // First read the 36 character UUID for the board from EEPROM
  for (int i = 0; i < 36; i++) {
    char part = EEPROM.read(i);
    UUID += String(part);
  }

  dht.begin();
}

void loop() {
  lightLevel = analogRead(LIGHTLEVELPIN);
  // Encode the data as JSON and send back to an altar-manager over serial
  Serial.print("{\"workerIdentifier\":\"" + UUID + "\",\"metricName\":\"lightLevel\",\"metricUnits\":\"lumens\",\"metricValue\":");
  Serial.print(lightLevel, DEC);
  Serial.println("}");

  float t = dht.readTemperature();
  Serial.print("{\"workerIdentifier\":\"" + UUID + "\",\"metricName\":\"airTemperature\",\"metricUnits\":\"°C\",\"metricValue\":");
  Serial.print(t, DEC);
  Serial.println("}");

  float h = dht.readHumidity();
  Serial.print("{\"workerIdentifier\":\"" + UUID + "\",\"metricName\":\"humidity\",\"metricUnits\":\"%\",\"metricValue\":");
  Serial.print(h, DEC);
  Serial.println("}");
  
  delay(5000);
}
