var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("./serviceSpecs/temperatureServiceZMQPUBSUB.json");

edisonLib.discoverServices(validator.getValidatedSpec(), function (serviceDescription) {
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