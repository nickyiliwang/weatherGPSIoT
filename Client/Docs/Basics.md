# Basics of IoT with AWS

## [MQTT with Certificate based Client Auth on Port 443](https://aws.amazon.com/about-aws/whats-new/2018/02/aws-iot-core-now-supports-mqtt-connections-with-certificate-based-client-authentication-on-port-443/)

MQTT is a lightweight, pub-sub network protocol that transports messages between devices. The protocol usually runs over TCP/IP, however, any network protocol that provides ordered, lossless, bi-directional connections can support MQTT

1. beneficial for deploying devices into environments where they do not control the IT infrastructure. 
2. MQTT client cert auth on TCP port 443/8883
3. Other notable combo is: HTTP SigV4 (AWS Signature Version 4) on port 443 (AWS specific, uses aws credentials)

## Getting the AWS iot endpoint
located at: aws console => AWS IoT => Settings

## Getting AWS certificates
1. Create a certificate at iot secure section
2. download the pub and pri keys
3. activate your certificate

## Subscribing to a topic
Once you have a publishing client, like the one on [hello-world.cpp](hello-../Archive/hello-world.cpp) 

1. go to console, locate MQTT test client
2. enter the topic name our client is publishing (ie. outTopic