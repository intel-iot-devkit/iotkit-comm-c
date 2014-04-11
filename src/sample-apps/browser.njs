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

discovery.discoverServices(serviceType, null, function(service, bestAddresses) {
	console.log("found " + service.type.name + " service at " +
			bestAddresses[0] + ":" + service.port);
});
