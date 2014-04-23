var edisonLib = require('../edison-lib');

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceZMQPUBSUB.json");

edisonLib.createService(validator.getValidatedDescription(), function (service) {
  "use strict";

  service.comm.setReceivedMessageHandler(function(message) {
    "use strict";
    console.log(message);
    service.comm.send(message); // echo server
  });

  setInterval(function () {
    "use strict";
    service.comm.send({topic: "mytopic", text: "my message"});
  }, 1000);

});