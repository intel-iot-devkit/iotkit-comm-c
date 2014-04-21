var edisonLib = require("../edison-lib");

function serviceFilter (serviceRecord, bestKnownAddresses) {
  "use strict";
  console.log("found " + serviceRecord.type.name + " service at " +
    bestKnownAddresses + ":" + serviceRecord.port + " on interface " + serviceRecord.networkInterface);
  if ("This is the service I want") {
    return bestKnownAddresses[0]; // you can always use another address from the service record
  } else {
    return false;
  }
}

edisonLib.createClient("./serviceSpecs/temperatureServiceZMQ.json", serviceFilter, function (client) {
  "use strict";

  client.comm.subscribe("mytopic");

  client.comm.setReceivedMessageHandler(function(message) {
    "use strict";
    console.log(message.toString());
  });
});


