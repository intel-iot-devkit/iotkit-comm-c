var mdns = require('mdns2');
var fs = require('fs');
var path = require('path');
var os = require('os');
var net = require('net');

var LOCAL_IP_STRING = "local";

// private functions and variables
var myaddresses = [];

// service cache to remove duplicate services
var serviceCache = {};

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
        myaddresses.push(address.address);
      }
    }
  }
}

function resolveToLocal(address) {
  "use strict";

  if (!address) {
    return address;
  }

  var isLocal = myaddresses.some(function (myaddress) {
    if (address === myaddress) {
      return true;
    }

    return false;

  });

  if (isLocal) {
    return LOCAL_IP_STRING;
  }

  return address;
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
  setMyAddresses();
  console.log(myaddresses);

	// todo: needs fix: multiple subtypes in the serviceType causes errors.
	// make sure your serviceType contains only *one* subtype
	var browser = mdns.createBrowser(serviceType, { resolverSequence: mdnsResolverSequence });

  browser.on('serviceUp', function(service) {

    if (!service.addresses) {
      return;
    }

    var address = service.addresses[0];
    var ip = resolveToLocal(address);

    console.log(address);
    console.log(ip);

    if (!serviceCache[service.name + ip + service.port]) {
      if (service.type.protocol === "tcp") {
        net.createConnection(service.port, address).on("connect", function() {
          serviceCache[service.name + ip + service.port] = {};
          callback(service);
          console.log("success");
        }).on("error", function(e) {
            // do nothing
            console.log(e);
          });
      } else {
        serviceCache[service.name + address + service.port] = {};
        callback(service);
      }
    }

  });

  browser.on('serviceDown', function(service) {
    "use strict";

    if (!service.addresses) {
      return;
    }

    service.addresses.forEach(function (address, i, thisArray) {
      "use strict";
      var ip = resolveToLocal(address);
      delete serviceCache[service.name + ip + service.port];
    });

  });

	browser.start();

};

EdisonMDNS.prototype.findService = function () {
	
};

// export the class
module.exports = EdisonMDNS;