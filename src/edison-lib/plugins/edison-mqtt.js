var mqtt = require('mqtt');

var client;

function EdisonMQTT(brokerip, brokerport, type, args) {
  "use strict";
    console.log(":IP:" + brokerip + ":port:"+brokerport+":type:"+type);
    if (type != 'undefined' && type == 'ssl') {
        console.log('going to create secure client');
        client = mqtt.createSecureClient(brokerport, brokerip, args);
    } else {
        console.log('going to create public client');
        client = mqtt.createClient(brokerport, brokerip);
    }
}

EdisonMQTT.prototype.component = "communication";
EdisonMQTT.prototype.name = "mqtt";

EdisonMQTT.prototype.send = function (topic, message) {
    client.publish(topic, message);
};

EdisonMQTT.prototype.subscribe = function (topic) {
    console.log(topic);
    client.subscribe(topic);
};

EdisonMQTT.prototype.unsubscribe = function (topic) {
  "use strict";
  console.log("unsubscribe mqtt.");
};

EdisonMQTT.prototype.setReceivedMessageHandler = function(callback) {
  "use strict";
  client.on('message', function (topic, message) {
    callback(topic, message);
  });
};

EdisonMQTT.prototype.done = function () {
	client.close();
};

module.exports = EdisonMQTT;