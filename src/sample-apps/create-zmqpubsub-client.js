var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceZMQ.json");

edisonLib.createClient(validator.getValidatedDescription(), serviceFilter, function (client) {
  "use strict";

  client.comm.subscribe("mytopic");

  client.comm.setReceivedMessageHandler(function(message) {
    "use strict";
    console.log(message.toString());
  });
});

function serviceFilter (serviceRecord) {
  "use strict";
  console.log("found " + serviceRecord.rawRecord.type.name + " service at " +
    serviceRecord.getSuggestedAddress() + ":" + serviceRecord.rawRecord.port + " on interface " + serviceRecord.rawRecord.networkInterface);

  if ("This is the service I want") {
    return serviceRecord.getSuggestedAddress(); // you can always use another address from the service record
  } else {
    return false;
  }
}