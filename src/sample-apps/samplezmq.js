//'use strict';

var edisonLib = require('../edison-lib');

var LocalDiscoveryService = edisonLib.getPlugin("discovery", "local");
var PubSubComm = edisonLib.getPlugin("communication", "pubsub");

var mdns = new LocalDiscoveryService();

var serviceType = {
		"name": "zmq",
		"protocol" : "tcp",
		"subtypes" : ["cpuTemp"]
};

var pubsubclient;

mdns.discoverServices(serviceType, onDiscovery);
function onDiscovery(service){
	console.log("found " + service.type.name + " service at " + service.addresses[service.addresses.length-1] + ":" + service.port);

    pubsubclient = new PubSubComm(service.addresses[service.addresses.length-1], service.port, 'sub');

	pubsubclient.subscribe('/Intel/temperature', function (topic, message) {
    console.log('msg ' + message);
    console.log('topic ' + topic);
  });
	
	console.log('waiting for messages !!');
}