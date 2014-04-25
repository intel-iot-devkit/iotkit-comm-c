var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceMQTT.json");

var serviceDescription = validator.getValidatedDescription();

edisonLib.createClientForGivenService(serviceDescription, function (client) {

  client.comm.subscribe(serviceDescription.name);

  client.comm.setReceivedMessageHandler(function(message, context) {
    "use strict";
    console.log(message);
  });

});