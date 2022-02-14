# Serverless Framework
working on a ideal lambda data format/transformation scheme to have local and forecasted weather separated as an response object

## about weatherGPSIoT function
This Lambda function is for the data transformation of incoming Client data from our ESP32

Once this functions is deployed into AWS in an CF template, we would need to select this function and added it as a pipeline activity ([see this Client doc for more info](../Client/docs/1.iot-analytics.md)) 

*important: we want to apply a resource-based permission policy to this function in order for iotanalytics to have permission to invoke this lambda function.

In a perfect world, we can attach this policy to the function in the serverless.yml but I haven't found a way to do it.

  resourcePolicy:
    # IoT Analytics 
    - Effect: Allow 
      Principal:
        Service:
          - iotanalytics.amazonaws.com
      Resource: ${self:functions.weatherGPSIoT}         	
      Action:
        - lambda:InvokeFunction

Instead we need to enter this command in the cli for this to work:

aws lambda add-permission --function-name <lambda-function-name> --statement-id <your-statement> --principal iotanalytics.amazonaws.com --action lambda:InvokeFunction --profile <your profile>

ie:
aws lambda add-permission --function-name weathergpsiot-dev-weatherGPSIoT --statement-id 1234 --principal iotanalytics.amazonaws.com --action lambda:InvokeFunction --profile iamadmin

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
