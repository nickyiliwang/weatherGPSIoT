#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <secrets.h>

#define DHTTYPE DHT11  // DHT 11

// DHT Sensor pin
uint8_t DHTPin = 4;

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

float Temperature;
float Humidity;

// Wifi
const char *ssid = ssid_name;
const char *password = ssid_password;
// endpoint
const char *awsEndpoint = my_awsEndpoint;
// certificates
const char *certificate_pem_crt = my_aws_certificate_pem_crt;
const char *private_pem_key = my_aws_private_pem_key;
const char *rootCA = my_aws_rootCA;

WiFiClientSecure wiFiClient;
void msgReceived(char *topic, byte *payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient);

void pubSubCheckConnect() {
  if (!pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: ");
    Serial.print(awsEndpoint);
    while (!pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthingXXXX");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

void updateTempHumid() {
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(DHTPin, INPUT);

  dht.begin();

  Serial.println();
  Serial.println("Weather Station");
  Serial.printf("SDK version: %s\n", ESP.getSdkVersion());

  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);
}

unsigned long lastPublish;
int msgCount;

void loop() {
  pubSubCheckConnect();

  // Change MQTT_MAX_PACKET_SIZE in PubSubClient.h for bigger buffer
  char iotData[128];
  updateTempHumid();

  // geolocation
  float latitude;
  float longitude;
  // RichmonHill, ON
  float gps[][2] = {{43.8828, -79.4403}};
  latitude = gps[0][0];
  longitude = gps[0][1];

  // payload
  snprintf(iotData, sizeof(iotData),
           "{\"uptime\":%lu,\"intemp\":%f,\"inhumid\":%f,\"lat\":%2.7f,"
           "\"long\":%3.7f}",
           millis() / 1000, Temperature, Humidity, latitude, longitude);

  if (millis() - lastPublish > 10000) {
    boolean rc = pubSubClient.publish("outTopic", iotData);
    Serial.print("Published, rc=");
    Serial.print((rc ? "OK: " : "FAILED: "));
    Serial.println(iotData);
    lastPublish = millis();
  }
}

void msgReceived(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message received on ");
  Serial.print(topic);
  Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
