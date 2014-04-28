var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("./serviceSpecs/temperatureServiceMQTT.json");

var serviceDescription = validator.getValidatedSpec();

edisonLib.createClientForGivenService(serviceDescription, function (client) {

  setInterval(function () {
    "use strict";
    client.comm.send("my other message", {topic: serviceDescription.name});
  }, 1000);

});