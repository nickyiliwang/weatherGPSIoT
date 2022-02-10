// We are sending this data through iot analytics pipline
// it pulls data from "outTopic", we are sending it like [{temperature: 12,
// humidity: 12}] (note the array) We are taking this data and transforming it
// with an lambda function that addes "serversideTimeStamp" , lambda function
// called: node_iot_esp32 *important iot analytics starts without permission to
// invoke lambda functions we had to add this to give it permissions:

// aws lambda add-permission --function-name node_iot_esp32
// --action lambda:InvokeFunction --statement-id iotanalytics
// --principal iotanalytics.amazonaws.com --profile iamadmin

// this sketch generates all the mock data for iot analytics and jupitor
// notebook/sagemake

// Using Dataset content delivery rules in aws iot ana to save data in csv in s3
// dataset/mydataset/!{iotanalytics:scheduleTime}/!{iotanalytics:versionId}.csv
// buckets needed to be public
// bucket policy
// {"Version":"2012-10-17","Statement":[{"Sid":"PublicRead","Effect":"Allow","Principal":"*","Action":"s3:GetObject","Resource":"arn:aws:s3:::iot-analytics-temperature-humidity-esp32-nickyiliwang/*"}]}
// cors
// [{"AllowedHeaders":["Authorization"],"AllowedMethods":["GET"],"AllowedOrigins":["*"],"ExposeHeaders":[],"MaxAgeSeconds":3000}]

#include <PubSubClient.h>
#include <WiFiClientSecure.h>
// sensitive
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

unsigned long lastPublish;
int msgCount;

void loop() {
  pubSubCheckConnect();

  // If you need to increase buffer size, then you need to change
  // MQTT_MAX_PACKET_SIZE in PubSubClient.h
  char fakeData[128];

  int t = random(30, 95);  // fake number range, adjust as you like
  int h = random(50, 95);

  snprintf(fakeData, sizeof(fakeData), "{\"temperature\":%d,\"humidity\":%d}",
           t, h);

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