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
			var ad = mdns.createAdvertisement(serviceSpec, serviceSpec.port, serviceSpec);
			//var ad = mdns.createAdvertisement(mdns.tcp('http'), 4321);
			ad.start();
			console.log("Advertised service specified in " + serviceSpecs[i]);
		}
	});
};

exports.discoverServices = function () {
};

exports.findService = function () {
	
};