var edisonLib = require('../edison-lib');

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceZMQPUBSUB.json");

edisonLib.createService(validator.getValidatedDescription(), function (service) {
  "use strict";

  setInterval(function () {
    "use strict";
    service.comm.publish("mytopic: my message");
  }, 1000);

});