var main = require("../main.njs");

function Client(serviceQuery, bestKnownAddress, servicePort) {
  "use strict";

  var commplugin;
  try {
    commplugin =  main.getPlugin("communication", serviceQuery.type.name);
  } catch (err) {
    console.log("ERROR: An appropriate communication plugin could not be found for service at '" +
      bestKnownAddress + "'. To interact with this service, the '" + serviceQuery.type.name +
      "' communication plugin is required.");
    throw err;
  }

  this.comm = new commplugin(servicePort, "client", bestKnownAddress);
}

Client.prototype.comm = null;

// export the class
module.exports = Client;