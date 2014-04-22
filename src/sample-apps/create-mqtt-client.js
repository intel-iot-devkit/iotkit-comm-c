var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureService.json");

edisonLib.createClientForGivenService(validator.getValidatedDescription(), function (client) {

  client.comm.subscribe("mytopic");

  client.comm.setReceivedMessageHandler(function(message) {
    "use strict";
    console.log(message);
  });

  setInterval(function () {
    "use strict";
    client.comm.send({topic: "mytopic", text: "my message"});
  }, 1000);

});