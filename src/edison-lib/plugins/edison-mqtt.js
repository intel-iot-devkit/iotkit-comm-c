var mqtt = require('mqtt');

var client;

function EdisonMQTT(brokerip, brokerport) {
  "use strict";

  console.log(__filename + ":IP:" + brokerip + ":port:"+brokerport);
  client = mqtt.createClient(brokerport, brokerip);
}

EdisonMQTT.prototype.components = ["communication"];
EdisonMQTT.prototype.name = "edisonMqtt";
EdisonMQTT.prototype.type = "pubsub";

EdisonMQTT.prototype.publish = function (topic, message) {
	client.publish(topic, message);
};

EdisonMQTT.prototype.subscribe = function (topic, callback) {
	client.subscribe(topic);
	
	client.on('message', function (topic, message) {
		  callback(topic, message);
		});
};

EdisonMQTT.prototype.close = function () {
	client.end();
};

module.exports = EdisonMQTT;