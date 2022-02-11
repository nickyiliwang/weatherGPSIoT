# Serverless Framework
working on a ideal lambda data format/transformation scheme to have local and forecasted weather separated as an response object

## Invoking lambda functions offline
aws lambda invoke /dev/null \
  --endpoint-url http://localhost:3002 \
  --function-name <your function name> \
  --payload <your payload> \
  --profile iamadmin \
  --cli-binary-format raw-in-base64-out

example:
aws lambda invoke /dev/null \
  --endpoint-url http://localhost:3002 \
  --function-name weathergpsiot-dev-weatherGPSIoT \
  --profile iamadmin \
  --payload '[{ "temperature": 51, "humidity": 34, "lat": 38.8977, "long": -77.0365 }]' \
  --cli-binary-format raw-in-base64-out
