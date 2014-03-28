//'use strict';

var edison = require('../edison-lib');
var path = require('path');
var fs = require('fs');

var edisonMdns = edison.localDirectory.edisonMdns;
var edisonMqtt = edison.localComm.edisonMqtt;

var serviceSpec = JSON.parse(fs.readFileSync(path.join(edison.config.libRoot, edison.config.serviceDir, "audioService.json")));

var client;

edisonMdns.discoverServices(serviceSpec.type, onDiscovery);
	
function onDiscovery(service){
	console.log("found " + service.type.name + " service at " + service.addresses[service.addresses.length-1] + ":" + service.port);
	
	client = edisonMqtt.createClient(service.addresses[service.addresses.length-1], service.port);
	
	client.subscribe('/Intel/temperature');
	
	console.log('waiting for messages !!');
	
	client.on('message', function (topic, message) {
	  console.log(message);
	});
}