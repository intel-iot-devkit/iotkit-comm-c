var edisonLib = require('edisonapi');

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("./serviceSpecs/temperatureServiceZMQREQREP.json");

edisonLib.createService(validator.getValidatedSpec(), function (service) {
  "use strict";

  service.comm.setReceivedMessageHandler(function(client, msg, context) {
    "use strict";
    console.log(msg.toString());
    service.comm.sendTo(client, "hi");
  });

});