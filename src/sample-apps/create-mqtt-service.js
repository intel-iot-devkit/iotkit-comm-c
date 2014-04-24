var edisonLib = require('../edison-lib');

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceMQTT.json");

edisonLib.createService(validator.getValidatedDescription(), function (service) {
  "use strict";

  service.comm.setReceivedMessageHandler(function(event, channel, msg) {
    "use strict";
    console.log(msg);
    service.comm.send(channel, msg); // echo server
  });

});