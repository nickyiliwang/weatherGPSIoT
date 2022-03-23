# IoT Local Weather Station
## Description:
This is the IoT side of my weather station project, the idea is using ESP32 micro controller with a temperature and humidity sensor, we can generate raw data and send it into AWS IoT Core for data analysis and processing. The raw data will then be formatted into a neat JSON format by lambda, deployed with serverless framework.Bby default the data we use are gonna render simple html graphs, later we are going to feed the formatted data into a terraform deployed private cloud running EC2 instances that will ingest the formatted data through an API and display it onto garfana.

So far we are separating the project into: 
[Client = ESP32 code](/Client/)
[Server = Serverless framework/Lambda code](/Server/)

Reference Docs:
[Server](Server/SERVER.md)
[Client](Client/CLIENT.md)

TODO:
1. Combine sensor 