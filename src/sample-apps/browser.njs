var edison = require('../edison-lib');

var edisonMdns = edison.localDirectory.edisonMdns;

var serviceType = {
		"name": "mqtt",
		"protocol" : "tcp",
		"subtypes" : ["cpuTemp"]
};

edisonMdns.discoverServices(serviceType, function(service) {
	console.log("found " + service.type.name + " service at " + service.addresses[service.addresses.length-1] + ":" + service.port);
});