var edisonLib = require("../edison-lib");

var query = new edisonLib.ServiceQuery();
query.initServiceQueryFromFile("./serviceQueries/temperatureServiceQueryMQTT.json");

edisonLib.discoverServices(query, function (serviceSpec) {
  "use strict";

  console.log("Found " + serviceSpec.type.name + " service at " +
    serviceSpec.address + ":" + serviceSpec.port + " on interface " +
    serviceSpec.networkInterface);

  edisonLib.createClientForGivenService(serviceSpec, function (client) {

    client.comm.subscribe("mytopic");

    client.comm.setReceivedMessageHandler(function(message) {
      "use strict";
      console.log(message.toString());
    });

    setInterval(function () {
      "use strict";
      client.comm.send("my message", {topic: "mytopic"});
    }, 1000);

  });
});