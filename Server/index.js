const serverless = require("serverless-http");
const express = require("express");
const cities = require("cities");
const weather = require("weather-js");
const app = express();
app.use(express.json());

app.get("/fuck", (req, res) => {
  res.json({
    statusCode: 420,
    body: "Fuck you Serverless, please",
  });
});

app.post("/", async (req, res) => {
  const { local } = req.body;

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
  local.forEach((message) => {
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
  console.log(responses);
  res.json(responses);
});

module.exports.handler = serverless(app);
