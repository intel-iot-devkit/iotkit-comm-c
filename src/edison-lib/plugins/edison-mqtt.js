exports.component = "localComm";
exports.name = "edisonMqtt";
exports.type = "pubsub";

var mqtt = require('mqtt');
var client;

exports.createClient = function (brokerip, brokerport) {
	
	console.log(__filename + ":IP:" + brokerip + ":port:"+brokerport);
	client = mqtt.createClient(brokerport, brokerip);
	
	return client;
};

exports.publish = function (topic, message) {
	client.publish(topic, message);
};

exports.subscribe = function (topic, callback) {
	client.subscribe(topic);
	
	client.on('message', function (topic, message) {
		  callback(topic, message);
		});
};

exports.close = function () {
	client.end();
};