var edisonLib = require('../edison-lib');

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceZMQREQREP.json");

edisonLib.createService(validator.getValidatedDescription(), function (service) {
  "use strict";

  service.comm.setReceivedMessageHandler(function(client, msg, context) {
    "use strict";
    console.log(msg.toString());
    service.comm.sendTo(client, "hi");
  });

});