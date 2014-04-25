var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceMQTT.json");

edisonLib.createClientForGivenService(validator.getValidatedDescription(), function (client) {

  client.comm.subscribe("mytopic");

  client.comm.setReceivedMessageHandler(function(message, context) {
    "use strict";
    console.log(message);
  });

  setInterval(function () {
    "use strict";
    client.comm.send("my message", {topic: "mytopic"});
  }, 1000);

});