const weather = require("weather-js");
const cities = require("cities");

module.exports.handler = async function (event, context) {
  let promiseData = [];

  let weatherPromise = function (data) {
    return new Promise((resolve, reject) => {
      weather.find(
        { search: `${data.State} ${data.City}`, degreeType: "F" },
        function (err, result) {
          if (err) {
            console.log("error weather", err);
            return;
          }

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
          data.weatherForecast = weatherTempArr;
          resolve(data);
        }
      );
    });
  };

  // payload shape: [{ "temperature": 51, "humidity": 34, "lat": 38.8977, "long": -77.0365 }]
  event.forEach((message) => {
    const citiesLookedUp = cities.gps_lookup(message.lat, message.long);

    // setting and adding dates
    const d = new Date();
    const n = d.toISOString();

    let data = {
      ...message,
      ...{
        City: citiesLookedUp.city,
        State: citiesLookedUp.state,
        Time: n,
      },
    };

    promiseData.push(weatherPromise(data));
  });

  const responses = await Promise.all(promiseData);

  return responses;
};

// responses shape:
//   [
//     {
//       temperature: 51,
//       humidity: 34,
//       lat: 39.0458,
//       long: -76.6413,
//       City: 'Gambrills',
//       State: 'Maryland',
//       Time: '2022-02-11T14:27:02.427Z',
//       weatherForecast: [ { temperature: '42', humidity: '68' } ]
//     }
//   ]

// ideal responses shape:
// [
//   {
//     // local weather
//     temperature: 51,
//     humidity: 34,
//     // local weather
//     lat: 39.0458,
//     long: -76.6413,
//     City: "Gambrills",
//     State: "Maryland",
//     Time: "2022-02-11T14:27:02.427Z",
//     weatherForecast: [{ temperature: "42", humidity: "68" }],
//   },
// ];
