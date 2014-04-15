//'use strict';

var edison = require('../edison-lib');
var path = require('path');
var fs = require('fs');

var EdisonMdns = edison.discovery.local;
var EdisonMqtt = edison.getPlugin("communication", "pubsub");

var serviceType = {
		"name": "mqtt",
		"protocol" : "tcp",
		"subtypes" : ["cpuTemp"]
};

var edisonMdns = new EdisonMdns();
edisonMdns.discoverServices(serviceType, null, onDiscovery);

var client;

function onDiscovery(service, bestAddresses){
	console.log("found " + service.type.name + " service at " + bestAddresses[0] + ":" + service.port);
	
	client = new EdisonMqtt(bestAddresses[0], service.port);
	client.subscribe('/Intel/temperature', function (topic, message) {
    "use strict";
    console.log(message);
  });

	console.log('waiting for messages !!');
}
