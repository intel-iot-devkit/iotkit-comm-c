var edison = require('../edison-lib');

var discoveryService = edison.localDirectory.basic;

var serviceType = {
		"name": "mqtt",
		"protocol" : "tcp",
		"subtypes" : ["cpuTemp"]
};

discoveryService.discoverServices(serviceType, function(service) {
	console.log("found " + service.type.name + " service at " +
			service.addresses[service.addresses.length-1] + ":" + service.port +
			" on interface " + service.networkInterface);
});
