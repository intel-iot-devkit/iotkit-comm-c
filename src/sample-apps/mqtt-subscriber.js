var edisonLib = require("edisonapi");

var query = new edisonLib.ServiceQuery();
query.initServiceQueryFromFile("./serviceQueries/temperatureServiceQueryMQTT.json");

edisonLib.createClient(query, serviceFilter, function (client) {

    client.comm.subscribe("mytopic");

    client.comm.setReceivedMessageHandler(function(message, context) {
        "use strict";
        console.log(message);
    });

});

function serviceFilter (serviceRecord) {
    "use strict";
    return true;
}