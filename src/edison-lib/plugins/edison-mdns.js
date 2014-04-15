var mdns = require('mdns2');
var fs = require('fs');
var path = require('path');
var os = require('os');
var net = require('net');

var LOCAL_ADDR = "127.0.0.1";

// private functions and variables
var myaddresses = [];

// service cache to remove duplicate services
var serviceCache = {};

// controls how service names are resolved to ip addresses
// see mdns2
var mdnsResolverSequence = [
  mdns.rst.DNSServiceResolve(),
  mdns.rst.getaddrinfo({ families: [4] }),
  mdns.rst.makeAddressesUnique()
];

function setMyAddresses() {
  var ifs = os.networkInterfaces();
  for (var i in ifs) {
    for (var j in ifs[i]) {
      var address = ifs[i][j];
      if (address.family === 'IPv4' && !address.internal) {
        myaddresses.push(address.address);
      }
    }
  }
}

function removeServiceFromCache(service) {
  "use strict";
  if (!service.name) {
    console.log("WARN: Cannot remove service. No name in service record. " +
      "The service originally intended to be removed will remain in cache.");
    return;
  }
  delete serviceCache[service.name];
}

function serviceIsLocal(serviceAddresses) {
  "use strict";

  if (!serviceAddresses || serviceAddresses.length == 0) {
    return false;
  }

  var done = serviceAddresses.some(function (serviceAddress) {
    var isLocal = myaddresses.some(function (myaddress) {
      if (serviceAddress === myaddress) {
        return true;
      }
      return false;
    });

    if (isLocal) {
      return true;
    }

    return false;

  });

  return done;
}

function getMatchingPrefixLen(serviceAddress, myaddress) {
  "use strict";
  var i = 0;
  while(i < serviceAddress.length && i < myaddress.length && serviceAddress[i] == myaddress[i]) {
    i++;
  }

  return i;
}

function getAddressesWithLongestPrefixMatch(serviceAddresses) {
  "use strict";
  var resultStore = {};

  serviceAddresses.forEach(function (serviceAddress) {
    myaddresses.forEach(function (myaddress) {
      var matchingPrefixLen = getMatchingPrefixLen(serviceAddress, myaddress);
      if (typeof resultStore[matchingPrefixLen] === 'undefined') {
        resultStore[matchingPrefixLen] = {};
      }
      if (typeof resultStore[matchingPrefixLen] === 'undefined') {
        resultStore[matchingPrefixLen] = {};
      }
      resultStore[matchingPrefixLen][serviceAddress] = true;
    });
  });

  var allPrefixLengths = Object.keys(resultStore);
  if (allPrefixLengths.length == 0) {
    return [];
  }
  allPrefixLengths = allPrefixLengths.map(Math.round);
  allPrefixLengths.sort(function(n1,n2){return n1 - n2});
  return Object.keys(resultStore[allPrefixLengths[allPrefixLengths.length-1]]);
}

function defaultServiceAddressFilter(service) {
  "use strict";

  if (!service.addresses || !service.name) {
    if (!service.name) {
      console.log("WARN: Discovered a service without a name. Dropping.");
    } else {
      console.log("WARN: Discovered a service without addresses. Dropping.");
    }
    return;
  }

  var notSeenBefore = [];
  service.addresses.forEach(function (address) {
    "use strict";
    if (!serviceCache[service.name]) {
      serviceCache[service.name] = true;
      notSeenBefore.push(address);
    }
  });

  if (notSeenBefore.length == 0) {
    return [];
  }

  if (serviceIsLocal(notSeenBefore)) {
    return [ LOCAL_ADDR ];
  }

  if (notSeenBefore.length == 1) {
    return [ notSeenBefore[0] ];
  }

  var longestPrefixMatches = getAddressesWithLongestPrefixMatch(notSeenBefore);
  longestPrefixMatches.sort(); // so we can return addresses in the same order for the same service. Necessary?

  return longestPrefixMatches;
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
			ad.start();
			console.log("Advertised service specified in " + serviceSpecs[i]);
		}
	});
};

EdisonMDNS.prototype.discoverServices = function (serviceType, serviceAddressFilter, callback) {

  setMyAddresses();
  var filterFunc = serviceAddressFilter ? serviceAddressFilter : defaultServiceAddressFilter;

	// todo: needs fix: multiple subtypes in the serviceType causes errors.
	// make sure your serviceType contains only *one* subtype
	var browser = mdns.createBrowser(serviceType, { resolverSequence: mdnsResolverSequence });

  browser.on('serviceUp', function(service) {
    var filteredServiceAddresses = filterFunc(service);
    if (filteredServiceAddresses.length != 0) {
      callback(service, filteredServiceAddresses);
    }
  });

  browser.on('serviceDown', function(service) {
    "use strict";
    removeServiceFromCache(service);
  });

	browser.start();

};

EdisonMDNS.prototype.findService = function () {
	
};

// export the class
module.exports = EdisonMDNS;