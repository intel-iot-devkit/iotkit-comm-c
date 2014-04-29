var edisonLib = require("../edison-lib");

var query = new edisonLib.ServiceQuery();
query.initServiceQueryFromFile("./serviceQueries/temperatureServiceQueryMQTT.json");

edisonLib.discoverServices(query, function (serviceQuery) {
  "use strict";

  console.log("Found " + serviceQuery.type.name + " service at " +
    serviceQuery.address + ":" + serviceQuery.port + " on interface " +
    serviceQuery.networkInterface);

});