exports.component = "localDirectory";
exports.name = "edison-mdns";

var mdns = require('mdns2');
var fs = require('fs');
var path = require('path');

var advertised = false;

exports.advertiseServices = function (serviceDirPath) {
	fs.readdir(serviceDirPath, function (err, serviceSpecs) {
		for (var i in serviceSpecs) {
			var serviceSpec = JSON.parse(fs.readFileSync(path.join(serviceDirPath, serviceSpecs[i])));
			var ad = mdns.createAdvertisement(serviceSpec.type, serviceSpec.port, {txtRecord: serviceSpec.properties});
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
		console.log("service up: ", service);
		callback(service);
	});

	browser.start();
};

exports.findService = function () {
	
};