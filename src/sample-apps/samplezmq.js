//'use strict';

var edison = require('../edison-lib');
var path = require('path');
var fs = require('fs');

var edisonMdns = edison.discovery.local;
var edisonZmq = edison.getPlugin("communication", "pubsub");

var serviceType = {
		"name": "zmq",
		"protocol" : "tcp",
		"subtypes" : ["cpuTemp"]
};


var client;

edisonMdns.discoverServices(serviceType, onDiscovery);
	
function onDiscovery(service){
	console.log("found " + service.type.name + " service at " + service.addresses[service.addresses.length-1] + ":" + service.port);
	
	client = edisonZmq.createClient(service.addresses[service.addresses.length-1], service.port);
	
	client.subscribe('/Intel/temperature');
	
	console.log('waiting for messages !!');
	
	client.on('message', function (message) {
	  console.log('msg' + message);
	});
}