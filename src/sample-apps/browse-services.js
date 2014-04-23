var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceZMQPUBSUB.json");

edisonLib.discoverServices(validator.getValidatedDescription(), function (serviceDescription) {
  "use strict";

  console.log("Found " + serviceDescription.type.name + " service at " +
    serviceDescription.address + ":" + serviceDescription.port + " on interface " +
    serviceDescription.networkInterface);

  edisonLib.createClientForGivenService(serviceDescription, function (client) {

    client.comm.subscribe("mytopic");

    client.comm.setReceivedMessageHandler(function(message) {
      "use strict";
      console.log(message.toString());
    });

  });
});