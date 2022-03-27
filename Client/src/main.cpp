#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WebServer.h>
#include <WiFi.h>

#include "secrets.h"

#define DHTTYPE DHT11  // DHT 11

/*Put your SSID & Password*/
const char* ssid = ssid_name;
const char* password = ssid_password;

// DHT Sensor pin
uint8_t DHTPin = 4;

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

float Temperature;
float Humidity;

void updateTempHumid() {
  Temperature = dht.readTemperature();  // Gets the values of the temperature
  Humidity = dht.readHumidity();        // Gets the values of the humidity

  Serial.println("Temperature is: ");
  Serial.println(Temperature);
  Serial.println("Humidity is: ");
  Serial.println(Humidity);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(DHTPin, INPUT);

  dht.begin();

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // connect to your local wi-fi network
  WiFi.begin(ssid, password);

  // check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected..!");

  Temperature = dht.readTemperature();  // Gets the values of the temperature
  Humidity = dht.readHumidity();        // Gets the values of the humidity
}

void loop() {
  updateTempHumid();
  delay(1000);
}