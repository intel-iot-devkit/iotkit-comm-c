var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceZMQREQREP.json");

edisonLib.createClient(validator.getValidatedDescription(), serviceFilter, function (client) {
  "use strict";

  client.comm.send("hello");

  client.comm.setReceivedMessageHandler(function(message, context) {
    "use strict";
    console.log(message.toString());
    client.comm.send("hello");
  });
});

function serviceFilter (serviceRecord) {
  "use strict";
  return true;
}