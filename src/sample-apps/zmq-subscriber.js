var edisonLib = require("../edison-lib");

var query = new edisonLib.ServiceQuery();
query.initServiceQueryFromFile("./serviceQueries/temperatureServiceQueryZMQPUBSUB.json");

edisonLib.createClient(query, serviceFilter, function (client) {
  "use strict";

  client.comm.subscribe("mytopic");

  client.comm.setReceivedMessageHandler(function(message, context) {
    "use strict";
    console.log(context);
    console.log(message.toString());
  });
});

function serviceFilter (serviceRecord) {
  "use strict";
  return true;
}