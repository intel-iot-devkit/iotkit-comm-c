var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceMQTT.json");

var serviceDescription = validator.getValidatedDescription();

edisonLib.createClientForGivenService(serviceDescription, function (client) {

  setInterval(function () {
    "use strict";
    client.comm.send("my message", {topic: serviceDescription.name});
  }, 1000);

});