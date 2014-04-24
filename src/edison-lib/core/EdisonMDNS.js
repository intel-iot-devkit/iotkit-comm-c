var mdns = require('mdns2');
var path = require('path');
var os = require('os');

var ServiceRecord = require("./ServiceRecord.js");

// private static
// service cache to remove duplicate services
exports.serviceCache = {};

exports.LOCAL_ADDR = "127.0.0.1";

// private functions and variables
exports.myaddresses = [];

// controls how service names are resolved to ip addresses
// see mdns2
exports.mdnsResolverSequence = [
  mdns.rst.DNSServiceResolve(),
  mdns.rst.getaddrinfo({ families: [4] }),
  mdns.rst.makeAddressesUnique()
];

// singleton class
function EdisonMDNS() {
  "use strict";
  setMyAddresses();
}

// public variables
EdisonMDNS.prototype.name = "mdns";
EdisonMDNS.prototype.component = "discovery";

// public functions
EdisonMDNS.prototype.advertiseService = function (serviceDescription) {
  var serviceRecord = new ServiceRecord(serviceDescription);
  var ad = mdns.createAdvertisement(serviceRecord.rawRecord.type, serviceRecord.rawRecord.port,
    {txtRecord: serviceRecord.rawRecord.properties, name: serviceRecord.rawRecord.name});
  ad.start();
};

EdisonMDNS.prototype.discoverServices = function (serviceQuery, serviceFilter, callback) {

  // todo: needs fix: multiple subtypes in the serviceType causes errors.
  // make sure your serviceType contains only *one* subtype
  var browser = mdns.createBrowser(serviceQuery.type, { resolverSequence: exports.mdnsResolverSequence });

  browser.on('serviceUp', function(service) {
    var filteredServiceAddresses = serviceAddressFilter(service);

    if (filteredServiceAddresses.length != 0) {

      var serviceRecord = new ServiceRecord();
      serviceRecord.initFromRawServiceRecord(service);
      serviceRecord.setSuggestedAddresses(filteredServiceAddresses);
      serviceRecord.setSuggestedAddress(filteredServiceAddresses[0]);

      if (!serviceFilter || serviceFilter(serviceRecord)) {
        try {
          callback(serviceRecord.getSuggestedServiceDescription());
        } catch (err) {
          return;
        }
      }
    }
  });

  browser.on('serviceDown', function(service) {
    "use strict";
    removeServiceFromCache(service);
  });

  browser.on('serviceChanged', function(service) {
    "use strict";
    // todo: correctly handle service changed. Check if address has changed. Deleting is not the answer since service changed is raised even when serviceup happens.
    //removeServiceFromCache(service);
  });

  browser.start();

};

// private functions
// NOTE: any variable in 'exports' becomes private static if module.exports is used as well.
function setMyAddresses() {
  var ifs = os.networkInterfaces();
  for (var i in ifs) {
    for (var j in ifs[i]) {
      var address = ifs[i][j];
      if (address.family === 'IPv4' && !address.internal) {
        exports.myaddresses.push(address.address);
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
  delete exports.serviceCache[service.name];
}

function serviceIsLocal(serviceAddresses) {
  "use strict";

  if (!serviceAddresses || serviceAddresses.length == 0) {
    return false;
  }

  return serviceAddresses.some(function (serviceAddress) {
    var isLocal = exports.myaddresses.some(function (myaddress) {
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
    exports.myaddresses.forEach(function (myaddress) {
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

function serviceAddressFilter(service) {
  "use strict";

  if (!service.addresses || !service.name) {
    if (!service.name) {
      console.log("WARN: Discovered a service without a name. Dropping.");
    } else {
      console.log("WARN: Discovered a service without addresses. Dropping.");
    }
    return [];
  }

  var notSeenBefore = [];
  service.addresses.forEach(function (address) {
    "use strict";
    if (typeof exports.serviceCache[service.name] === 'undefined') {
      exports.serviceCache[service.name] = {};
    }
    if (!exports.serviceCache[service.name][address]) {
      exports.serviceCache[service.name][address] = true;
      notSeenBefore.push(address);
    }
  });

  if (notSeenBefore.length == 0) {
    return [];
  }

  if (serviceIsLocal(Object.keys(exports.serviceCache[service.name]))) {
    return [ exports.LOCAL_ADDR ];
  }

  if (notSeenBefore.length == 1) {
    return [ notSeenBefore[0] ];
  }

  var longestPrefixMatches = getAddressesWithLongestPrefixMatch(notSeenBefore);
  longestPrefixMatches.sort(); // so we can return addresses in the same order for the same service. Necessary?

  return longestPrefixMatches;
}

// export the class
module.exports = new EdisonMDNS(); // must be at the end
