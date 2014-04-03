var mqtt = require('mqtt');

var client;

function EdisonMQTT(brokerip, brokerport, type) {
  "use strict";
    console.log(__filename + ":IP:" + brokerip + ":port:"+brokerport);
    if (type == 'pub') {
        client = mqtt.createClient(brokerip, brokerport);
    } else {
        client = mqtt.createClient(brokerip, brokerport);
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