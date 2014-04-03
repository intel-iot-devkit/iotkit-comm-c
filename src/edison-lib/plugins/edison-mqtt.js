var mqtt = require('mqtt');

var publishDest;
var subscriptionSource;

function EdisonMQTT() {
  "use strict";
}

EdisonMQTT.prototype.components = ["communication"];
EdisonMQTT.prototype.name = "edisonMqtt";
EdisonMQTT.prototype.type = "pubsub";

EdisonMQTT.prototype.publishTo = function (brokerip, brokerport) {
  "use strict";
  console.log(__filename + ":IP:" + brokerip + ":port:"+brokerport);
  publishDest = mqtt.createClient(brokerip, brokerport);
}

EdisonMQTT.prototype.subscribeFrom = function (brokerip, brokerport) {
  "use strict";
  subscriptionSource = mqtt.createClient(brokerip, brokerport);
}

EdisonMQTT.prototype.publish = function (topic, message) {
	publishDest.publish(topic, message);
};

EdisonMQTT.prototype.subscribe = function (topic, callback) {
  console.log(topic);
	subscriptionSource.subscribe(topic);
	
	subscriptionSource.on('message', function (topic, message) {
		  callback(topic, message);
		});
};

EdisonMQTT.prototype.close = function () {
	client.end();
};

module.exports = EdisonMQTT;