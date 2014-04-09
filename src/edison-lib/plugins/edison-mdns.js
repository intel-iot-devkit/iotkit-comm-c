var mdns = require('mdns2');
var fs = require('fs');
var path = require('path');
var os = require('os');

// private functions and variables
var addresses = [];
// controls how service names are resolved to ip addresses
// see mdns2
var mdnsResolverSequence = [
  mdns.rst.DNSServiceResolve(),
  mdns.rst.getaddrinfo({ families: [4] })
];

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

// class
function EdisonMDNS() {
  "use strict";
}

// public variables
EdisonMDNS.prototype.components = ["discovery"];
EdisonMDNS.prototype.name = "edisonMdns";
EdisonMDNS.prototype.type = "local";

// public functions
EdisonMDNS.prototype.advertiseServices = function (serviceDirPath) {
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

EdisonMDNS.prototype.discoverServices = function (serviceType, callback) {
	// todo: needs fix: multiple subtypes in the serviceType causes errors.
	// make sure your serviceType contains only *one* subtype
	var browser = mdns.createBrowser(serviceType, { resolverSequence: mdnsResolverSequence });
	
	browser.on('serviceUp', function(service) {
		
		callback(service);
	});

	browser.start();
};

EdisonMDNS.prototype.findService = function () {
	
};

// export the class
module.exports = EdisonMDNS;