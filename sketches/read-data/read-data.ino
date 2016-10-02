#include <EEPROM.h>
#include "DHT.h"
#include <PWM.h>
int32_t frequency = 30000; // desired frequency in Hertz

#define LIGHTLEVELPIN A0
#define EXHAUSTPWMPIN 10
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

int lightLevel;
String UUID = "";

String content = "";
int pwmValue = 0;
char character;

int fanPulse = 0;
unsigned long pulseDuration;

void setup() {
  InitTimersSafe();
  bool success = SetPinFrequencySafe(10, frequency);
  Serial.begin(38400);
  // First read the 36 character UUID for the board from EEPROM
  for (int i = 0; i < 36; i++) {
    char part = EEPROM.read(i);
    UUID += String(part);
  }

  dht.begin();

  // Set the PWM pins as output.
  pinMode(7, INPUT);
  pinMode(EXHAUSTPWMPIN, OUTPUT); // sets the pin as output
}

void loop() {

  if (Serial.available()) {
    String commandTarget = "";
    String commandValue = "";

    // Read in external commands to set internal values in the form key:value\0
    commandTarget = Serial.readStringUntil(':');
    commandValue = Serial.readStringUntil('\0');

    if (commandTarget == "exhaustFanSpeed") {
      Serial.println(commandTarget);
      Serial.println(commandValue);
      pwmWrite(EXHAUSTPWMPIN, commandValue.toInt());
    }
  }

  // READ METRICS
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

  pulseDuration = pulseIn(7, HIGH);
  double frequencyRead = 1000000/pulseDuration;
  float exhaustRPM = frequencyRead / 2 * 60;
  // Encode the data as JSON and send back to an altar-manager over serial
  Serial.print("{\"workerIdentifier\":\"" + UUID + "\",\"metricName\":\"exhaustFanSpeed\",\"metricUnits\":\"RPM\",\"metricValue\":");
  Serial.print(exhaustRPM, DEC);
  Serial.println("}");
  
  delay(5000);
}
