#include <Arduino.h>

#include <EEPROM.h>
#include "DHT.h"
#include <PWM.h>
int32_t frequency = 30000; // desired frequency in Hertz

#define LIGHTLEVELPIN A0
#define EXHAUSTPWMPIN 10
#define INTAKEPWMPIN 9
#define INTAKESPEEDPIN 8
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

int lightLevel;
String UUID = "";

String content = "";
int pwmValue = 0;
char character;

void setup() {
  InitTimersSafe();
  SetPinFrequencySafe(EXHAUSTPWMPIN, frequency);
  SetPinFrequencySafe(INTAKEPWMPIN, frequency);
  Serial.begin(38400);
  // First read the 36 character UUID for the board from EEPROM
  for (int i = 0; i < 36; i++) {
    char part = EEPROM.read(i);
    UUID += String(part);
  }

  dht.begin();

  // Set the PWM pins as output.
  pinMode(7, INPUT);
  pinMode(INTAKESPEEDPIN, INPUT);
  pinMode(EXHAUSTPWMPIN, OUTPUT); // sets the pin as output
  pinMode(INTAKEPWMPIN, OUTPUT); // sets the pin as output

  pwmWrite(EXHAUSTPWMPIN, 255);
  pwmWrite(INTAKEPWMPIN, 255);
}

void loop() {

  if (Serial.available()) {
    String commandTarget = "";
    String commandValue = "";

    // Read in external commands to set internal values in the form key:value\0
    commandTarget = Serial.readStringUntil(':');
    commandValue = Serial.readStringUntil('\0');

    if (commandTarget == "exhaustFanSpeed") {
      pwmWrite(EXHAUSTPWMPIN, commandValue.toInt());
    }
    else if (commandTarget == "intakeFanSpeed") {
      pwmWrite(INTAKEPWMPIN, commandValue.toInt());
    }
  }

  Serial.println("{\"type\":\"identification\",\"value\":\"altar-worker\"}");

  // READ METRICS
  lightLevel = analogRead(LIGHTLEVELPIN);
  if (lightLevel != NAN && !isnan(lightLevel)) {
    // Encode the data as JSON and send back to an altar-manager over serial
    Serial.print("{\"workerIdentifier\":\"" + UUID + "\",\"metricName\":\"lightLevel\",\"metricUnits\":\"lumens\",\"metricValue\":");
    Serial.print(lightLevel, DEC);
    Serial.println("}");
  }

  float t = dht.readTemperature();
  if (t != NAN && !isnan(t)) {
    Serial.print("{\"workerIdentifier\":\"" + UUID + "\",\"metricName\":\"airTemperature\",\"metricUnits\":\"degreesCelcius\",\"metricValue\":");
    Serial.print(t, DEC);
    Serial.println("}");
}

  float h = dht.readHumidity();
  if (h != NAN && !isnan(h)) {
    Serial.print("{\"workerIdentifier\":\"" + UUID + "\",\"metricName\":\"humidity\",\"metricUnits\":\"percent\",\"metricValue\":");
    Serial.print(h, DEC);
    Serial.println("}");
  }

  unsigned long pulseDuration = pulseIn(7, HIGH);
  double frequencyRead = 1000000/pulseDuration;
  float exhaustRPM = frequencyRead / 4 * 60;
  if (pulseDuration != 0) {
    // Encode the data as JSON and send back to an altar-manager over serial
    Serial.print("{\"workerIdentifier\":\"" + UUID + "\",\"metricName\":\"exhaustFanSpeed\",\"metricUnits\":\"rpm\",\"metricValue\":");
    Serial.print(exhaustRPM, DEC);
    Serial.println("}");
  }

  pulseDuration = pulseIn(INTAKESPEEDPIN, HIGH);
  frequencyRead = 1000000/pulseDuration;
  float intakeRPM = frequencyRead / 4 * 60;
  if (pulseDuration != 0) {
    // Encode the data as JSON and send back to an altar-manager over serial
    Serial.print("{\"workerIdentifier\":\"" + UUID + "\",\"metricName\":\"intakeFanSpeed\",\"metricUnits\":\"rpm\",\"metricValue\":");
    Serial.print(intakeRPM, DEC);
    Serial.println("}");
  }

  delay(5000);
}
