exports.component = "localDirectory";
exports.name = "edisonMdns";
exports.type = "basic";

var mdns = require('mdns2');
var fs = require('fs');
var path = require('path');
var os = require('os');

var addresses = [];

//credits: http://stackoverflow.com/questions/10750303/how-can-i-get-the-local-ip-address-in-node-js
function setMyAddresses() {
	var interfaces = os.networkInterfaces();
	for (var k in interfaces) {
		for (var k2 in interfaces[k]) {
			var address = interfaces[k][k2];
			if (address.family === 'IPv4' && !address.internal) {
				addresses.push(address.address);
			}
		}
	}
}

// exports
exports.advertiseServices = function (serviceDirPath) {
	fs.readdir(serviceDirPath, function (err, serviceSpecs) {
		for (var i in serviceSpecs) {
			var serviceSpec = JSON.parse(fs.readFileSync(path.join(serviceDirPath, serviceSpecs[i])));
			var ad = mdns.createAdvertisement(serviceSpec.type, serviceSpec.port,
					{txtRecord: serviceSpec.properties, name: serviceSpec.name});
			//var ad = mdns.createAdvertisement(mdns.tcp('http'), 4321);
			ad.start();
			console.log("Advertised service specified in " + serviceSpecs[i]);
		}
	});
};

exports.discoverServices = function (serviceType, callback) {	
	// todo: needs fix: multiple subtypes in the serviceType causes errors.
	// make sure your serviceType contains only *one* subtype
	var browser = mdns.createBrowser(serviceType);
	
	browser.on('serviceUp', function(service) {
		
		callback(service);
	});

	browser.start();
};

exports.findService = function () {
	
};