var path = require('path');

var spawn = require('child_process').spawn;

var mqttservice;

describe.only('mdns:', function () {

  before(function () {
    "use strict";
    mqttservice = spawn('node', [path.join(__dirname, 'mqtt-mini-broadcast-broker.js')]);
    console.log(mqttservice);
  });

//  after(function() {
//    "use strict";
//    console.log("killed");
//    mqttservice.kill();
//  });

  it("should be able to find a service for the given query", function(done) {
    console.log("test started");
    var edisonLib = require('edisonapi');

    var query = new edisonLib.ServiceQuery();
    query.initServiceQueryFromFile(path.join(__dirname, "serviceQueries/temperatureServiceQueryMQTT.json"));

    edisonLib.discoverServices(query, function (serviceQuery) {
      "use strict";

      console.log("Found " + serviceQuery.type.name + " service at " +
        serviceQuery.address + ":" + serviceQuery.port + " on interface " +
        serviceQuery.networkInterface);

      done();
    });
  });
});

