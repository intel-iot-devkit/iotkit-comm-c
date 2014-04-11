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

EdisonMQTT.prototype.components = ["communication"];
EdisonMQTT.prototype.name = "edisonMqtt";
EdisonMQTT.prototype.type = "pubsub";

EdisonMQTT.prototype.publish = function (topic, message) {
    client.publish(topic, message);
};

EdisonMQTT.prototype.subscribe = function (topic, callback) {
    console.log(topic);
    client.subscribe(topic);

    client.on('message', function (topic, message) {
		  callback(topic, message);
		});
};

EdisonMQTT.prototype.close = function () {
	client.close();
};

module.exports = EdisonMQTT;