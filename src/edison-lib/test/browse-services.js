var path = require('path');

var spawn = require('child_process').spawn;

var mqttservice;

describe('[browse-services]', function () {

  before(function () {
    "use strict";
    mqttservice = spawn('node', [path.join(__dirname, 'mqtt-mini-broadcast-broker.js'), 'dependency']);
  });

  it("should find a service for the given query", function(done) {
    var edisonLib = require('edisonapi');

    var query = new edisonLib.ServiceQuery();
    query.initServiceQueryFromFile(path.join(__dirname, "serviceQueries/temperatureServiceQueryMQTT.json"));

    edisonLib.discoverServices(query, function (serviceSpec) {
      "use strict";
      done();
    });
  });
});

