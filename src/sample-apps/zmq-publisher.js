var edisonLib = require('edisonapi');

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("./serviceSpecs/temperatureServiceZMQPUBSUB.json");

edisonLib.createService(validator.getValidatedSpec(), function (service) {
  "use strict";

  setInterval(function () {
    "use strict";
    service.comm.publish("mytopic: my message", {});
  }, 1000);

});