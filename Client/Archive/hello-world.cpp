/* ESP32 AWS IoT
 *
 * Simplest possible example (that I could come up with) of using an ESP32 with
 * AWS IoT.
 *
 * Author: Anthony Elder
 * License: Apache License v2
 */

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#include "secrets.h"

const char* ssid = ssid_name;
const char* password = ssid_password;
// endpoint
const char* awsEndpoint = my_awsEndpoint;
// certificates
const char* certificate_pem_crt = my_aws_certificate_pem_crt;
const char* private_pem_key = my_aws_private_pem_key;
const char* rootCA = my_aws_rootCA;

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient);

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println();
  Serial.println("ESP32 AWS IoT Example");
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

unsigned long lastPublish;
int msgCount;

void loop() {
  pubSubCheckConnect();

  if (millis() - lastPublish > 10000) {
    String msg = String("Hello from Nick's ESP32: ") + ++msgCount;
    boolean rc = pubSubClient.publish("outTopic", msg.c_str());
    Serial.print("Published, rc=");
    Serial.print((rc ? "OK: " : "FAILED: "));
    Serial.println(msg);
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
