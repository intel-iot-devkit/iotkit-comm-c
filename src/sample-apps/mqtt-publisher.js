var edisonLib = require("edisonapi");

var query = new edisonLib.ServiceQuery();
query.initServiceQueryFromFile("./serviceQueries/temperatureServiceQueryMQTT.json");

edisonLib.createClient(query, serviceFilter, function (client) {

  setInterval(function () {
    "use strict";
    client.comm.send("my other message", {topic: "mytopic"});
  }, 1000);

});

function serviceFilter (serviceRecord) {
    "use strict";
    return true;
}