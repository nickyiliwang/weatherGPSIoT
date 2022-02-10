const serverless = require("serverless-http");
const express = require("express");
const weather = require("weather-js");
const cities = require("cities");
const app = express();
// app.use(express.json());

app.get("/city", (req, res) => {
  const city = cities.findByState("CA")[1].city; // LA
  weather.find(
    { search: "Los Angeles, CA", degreeType: "F" },
    function (err, result) {
      if (err) console.log(err);
      let currentTemp = result[0].current.temperature;
      // console.log(JSON.stringify(result, null, 2));

      res.send(`Hello from ${city}, the current temp is ${currentTemp} °F`);
    }
  );
});

module.exports.handler = serverless(app);
