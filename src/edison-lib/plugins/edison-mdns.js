exports.type = "localdiscovery";
exports.name = "edison-mdns";

//var mdns = require('mdns');
var fs = require('fs');

var advertised = false;

exports.advertiseServices = function (serviceDirPath) {
	fs.readdir(serviceDirPath, function (err, serviceSpecs) {
		for (var i in serviceSpecs) {
			console.log(serviceSpecs[i]);
		}
	});
};

exports.discoverServices = function () {
};

exports.findService = function () {
	
};