var edisonLib = require("../../edison-lib");

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("../serviceSpecs/IOTKitCloudBrokerSubOnly.json");
var brokerSpec = validator.getValidatedSpec();

var count;

edisonLib.createClientForGivenService(brokerSpec, function (client) {

  client.comm.subscribe(brokerSpec.name + "/b8-e8-56-37-7a-55");
  client.comm.subscribe(brokerSpec.name + "/b8-e8-56-37-7a-33");

  client.comm.setReceivedMessageHandler(function(message, context) {
    "use strict";

    var m = JSON.parse(message);
    if (m.data_source[0].metrics[0].name === 'love'
      || m.data_source[0].metrics[0].name === 'steps') {
      console.log(m.data_source[0].metrics[0]);
    }

  });

});