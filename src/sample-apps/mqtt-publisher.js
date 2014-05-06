var edisonLib = require("edisonapi");

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("./serviceSpecs/temperatureServiceMQTTBROKER.json");

var serviceSpec = validator.getValidatedSpec();

edisonLib.createClientForGivenService(serviceSpec, function (client) {

  setInterval(function () {
    "use strict";
    client.comm.send("my other message", {topic: serviceSpec.name});
  }, 1000);

});