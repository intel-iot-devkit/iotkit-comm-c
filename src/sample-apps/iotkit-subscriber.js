
// Direct client application to IoT Broker; does NOT go through IoTKit-Agent

var edisonLib = require("edisonapi");

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("./serviceQueries/temperatureSubscriberIoTKit.json");
var brokerSpec = validator.getValidatedSpec();

edisonLib.createClientForGivenService(brokerSpec, function (client) {
    client.comm.setReceivedMessageHandler(function(message, context) {
        "use strict";
        console.log(message);
    });

    client.comm.subscribe("server/metric/43d7606c-4f07-4f3b-958a-974c4a403039/f0-de-f1-e4-75-bb");
});
