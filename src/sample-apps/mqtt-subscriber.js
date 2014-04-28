var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("./serviceSpecs/temperatureServiceMQTT.json");

var serviceDescription = validator.getValidatedSpec();

edisonLib.createClientForGivenService(serviceDescription, function (client) {

  client.comm.subscribe(serviceDescription.name);

  client.comm.setReceivedMessageHandler(function(message, context) {
    "use strict";
    console.log(message);
  });

});