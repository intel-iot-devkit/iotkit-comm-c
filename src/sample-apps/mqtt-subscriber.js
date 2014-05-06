var edisonLib = require("edisonapi");

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("./serviceSpecs/temperatureServiceMQTTBROKER.json");

var serviceSpec = validator.getValidatedSpec();

edisonLib.createClientForGivenService(serviceSpec, function (client) {

  client.comm.subscribe(serviceSpec.name);

  client.comm.setReceivedMessageHandler(function(message, context) {
    "use strict";
    console.log(message);
  });

});