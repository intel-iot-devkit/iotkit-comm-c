var edisonLib = require("edisonapi");

var query = new edisonLib.ServiceQuery();
query.initServiceQueryFromFile("./serviceQueries/temperatureServiceQueryMQTT.json");

edisonLib.discoverServices(query, function (serviceSpec) {
  "use strict";

  console.log("Found " + serviceSpec.type.name + " service at " +
    serviceSpec.address + ":" + serviceSpec.port + " on interface " +
    serviceSpec.networkInterface);

});