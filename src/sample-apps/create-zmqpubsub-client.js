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
  return true;
}