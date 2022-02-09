//Rename this file 'index.js'
//this lambda for extracting csv data from your s3 bucket
//.csv to json object conversion for High Charts
//to browse your bucket schema use https://bucket-name.s3.your-aws-region.amazonaws.com/your-key-name
//example public bucket: https://brand-new-bucket-nickyiliwang.s3.us-east-1.amazonaws.com
//private bucket: arn:aws:s3:::weather-gps-iot-nickyiliwang
//keys = dataobjects within s3 bucket

const AWS = require("aws-sdk");

module.exports.handler = async function (event, context) {
  const s3 = new AWS.S3({ apiVersion: "2006-03-01" });
  var params = {
    Bucket: "weather-gps-iot-nickyiliwang",
    MaxKeys: 50,
  };

  function csvJSON(csv) {
    var lines = csv.split("\n");

    var result = [];
    var headers = lines[0].split(",");

    for (var i = 1; i < lines.length; i++) {
      if (lines[i].length > 0) {
        var weatherData = lines[i].substring(
          lines[i].lastIndexOf("[") + 1,
          lines[i].lastIndexOf("]")
        );
        weatherData = `${weatherData}`;
        lines[i] = lines[i].replace(`${weatherData}`, "WEATHER_$$$_DATA");
        var obj = {};
        var currentline = lines[i].split(",");

        for (var j = 0; j < headers.length; j++) {
          headers[j] = headers[j].replace(/"/g, "");
          if (headers[j] == "weather") {
            let splitArr = weatherData.replace(/}/, "").split(",");
            let weatherFirstObj = {};
            if (splitArr.length >= 2) {
              weatherFirstObj.temperature = splitArr[0].split("=")[1];
              weatherFirstObj.humidity = splitArr[1].split("=")[1];
            }
            obj[headers[j]] = weatherFirstObj;
          } else {
            obj[headers[j]] = currentline[j]
              ? currentline[j].replace(/"/g, "")
              : "";
          }
        }
        result.push(obj);
      }
    }
    console.log("JSON.stringify(result);", JSON.stringify(result));
    return result;
  }

  return new Promise((resolve, reject) => {
    // Common ways of extracting object from a bucket
    s3.listObjects(params, (err, objects) => {
      if (err) return console.log(err);
      if (objects && objects.Contents && objects.Contents.length > 0) {
        let contents = objects.Contents;
        contents.sort(function (a, b) {
          return new Date(b.LastModified) - new Date(a.LastModified);
        });

        let latestCsvData = contents[0];
        var params1 = {
          Bucket: "weather-gps-iot-nickyiliwang",
          Key: latestCsvData.Key,
        };
        s3.getObject(params1, (err, data) => {
          if (err) {
            console.log(err);
            reject(false);
          } else {
            try {
              var dataArray = data.Body.toString("utf-8");
              resolve(csvJSON(dataArray));
            } catch (error) {
              console.log(error);
              reject(false);
            }
          }
        });
      }
    });
  });
};
