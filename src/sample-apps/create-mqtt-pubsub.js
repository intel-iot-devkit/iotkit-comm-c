var edisonLib = require("../edison-lib");

var validator = new edisonLib.ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("./serviceSpecs/temperatureServiceMQTT.json");

edisonLib.createClientForGivenService(validator.getValidatedDescription(), function (client) {

  client.comm.subscribe("mytopic");

  client.comm.setReceivedMessageHandler(function(event, source, message) {
    "use strict";
    console.log(message);
  });

  setInterval(function () {
    "use strict";
    client.comm.send(client.comm.getDefaultChannel(), {topic: "mytopic", text: "my message"});
  }, 1000);

});