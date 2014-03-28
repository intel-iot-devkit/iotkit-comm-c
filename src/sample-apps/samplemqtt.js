//'use strict';

var edison = require('../edison-lib');
var path = require('path');
var fs = require('fs');

var serviceSpec = JSON.parse(fs.readFileSync(path.join(edison.config.libRoot, edison.config.serviceDir, "audioService.json")));

var client;

edison.localDirectory.discoverServices(serviceSpec.type, onDiscovery);


function onDiscovery(service){
	console.log("found " + service.type.name + " service at " + service.addresses[service.addresses.length-1] + ":" + service.port);
	
	client = edison.localComm.createClient(service.port, service.addresses[service.addresses.length-1]);
	
	client.subscribe('temperature');
	
	console.log('waiting for messages !!');
	
	client.on('message', function (topic, message) {
	  console.log(message);
	});
}