var edisonlib = require('../edison-lib');
var os = require('os');

var DiscoveryService = edisonlib.getPlugin("discovery", "local");
var discovery = new DiscoveryService();

//console.log(os.networkInterfaces());

var serviceType = {
		"name": "mqtt",
		"protocol" : "tcp",
		"subtypes" : ["cpuTemp"]
};

discovery.discoverServices(serviceType, function(service) {
	console.log("found " + service.type.name + " service at " +
			service.addresses[service.addresses.length-1] + ":" + service.port +
			" on interface " + service.networkInterface);
});
