#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
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

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println();
  Serial.println("ESP32 AWS GPS Weather Example by SDB for Udemy");
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

  // If you need to increase buffer size, then you need to change
  // MQTT_MAX_PACKET_SIZE in PubSubClient.h
  char fakeData[128];
  int t = random(30, 110);  // fake number range, adjust as you like
  int h = random(40, 100);

  int cityNumber =
      random(0, 4);  // range is 0- total # cities, range gets rounded down by
                     // random function so result is always n-1
  float latt;
  float lon;
  // Moab, Tuscon, Chicago, Stevenson
  float gps[][2] = {{38.5743966, -109.5689282},
                    {32.1558328, -111.0238918},
                    {41.881832, -87.623177},
                    {45.6944496, -121.9115935}};
  latt = gps[cityNumber][0];
  lon = gps[cityNumber][1];

  // Don't overflow your buffer! use short names and data types, the MQTT
  // library has package size limitations per cycle
  snprintf(fakeData, sizeof(fakeData),
           "{\"uptime\":%lu,\"intemp\":%d,\"inhumid\":%d,\"lat\":%2.7f,"
           "\"long\":%3.7f}",
           millis() / 1000, t, h, latt, lon);

  if (millis() - lastPublish > 10000) {
    boolean rc = pubSubClient.publish("outTopic", fakeData);
    Serial.print("Published, rc=");
    Serial.print((rc ? "OK: " : "FAILED: "));
    Serial.println(fakeData);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on ");
  Serial.print(topic);
  Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
