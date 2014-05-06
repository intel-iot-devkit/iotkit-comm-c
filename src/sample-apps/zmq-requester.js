var edisonLib = require("edisonapi");

var query = new edisonLib.ServiceQuery();
query.initServiceQueryFromFile("./serviceQueries/temperatureServiceQueryZMQREQREP.json");

edisonLib.createClient(query, serviceFilter, function (client) {
  "use strict";

  client.comm.send("hello");

  client.comm.setReceivedMessageHandler(function(message, context) {
    "use strict";
    console.log(message.toString());
    client.comm.send("hello");
  });
});

function serviceFilter (serviceRecord) {
  "use strict";
  return true;
}