const weather = require("weather-js");
const cities = require("cities");

module.exports.weatherGPSIoT = async function (event, context) {
  let promiseData = [];

  let weatherPromise = function (data) {
    return new Promise((resolve, reject) => {
      weather.find(
        { search: `${data.State} ${data.City}`, degreeType: "F" },
        function (err, result) {
          if (err) console.log("error weather", err);

          let weatherTempArr = [];
          if (result) {
            result.forEach((item) => {
              console.log(item.current.temperature);
              weatherTempArr.push({
                temperature: item.current.temperature,
                humidity: item.current.humidity,
              });
            });
          }
          data.weather = weatherTempArr;
          resolve(data);
        }
      );
    });
  };

  event.forEach((message) => {
    // do some stuff to `message`
    //use incoming latitude nd longitude from event object to get city and state
    const obj = cities.gps_lookup(message.lat, message.long);

    //get ISO8601 date formate. natively supported in browser JS
    var d = new Date();
    var n = d.toISOString(); //move time-stamping function to local scope to prevent 'warm start' issues, and memory heap TSR issue

    /******************* get current weather by using city and state fromm cities.js *******************/
    let data = Object.assign({}, message, {
      City: obj.city,
      State: obj.state,
      Time: n,
    });
    promiseData.push(weatherPromise(data));
    /******************* end *******************/
  }); //end for each

  const responses = await Promise.all(promiseData);
  return responses;
};

// permission
// aws lambda add-permission --function-name weathergpsiot-dev-weatherGPSIoT --statement-id 0710 --principal iotanalytics.amazonaws.com --action lambda:InvokeFunction
