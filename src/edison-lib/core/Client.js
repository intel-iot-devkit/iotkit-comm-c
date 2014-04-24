var main = require("../main.njs");


Client.prototype.comm = null;
Client.prototype.serviceDescription = null;

function Client(serviceDescription) {
  "use strict";

  this.serviceDescription = serviceDescription;

  var commplugin;
  try {
    commplugin =  main.getClientPlugin(serviceDescription.type.name);
  } catch (err) {
    console.log("ERROR: An appropriate communication plugin could not be found for service at '" +
      serviceDescription.address + "'. To interact with this service the '" + serviceDescription.type.name +
      "' plugin is required.");
    console.log(err);
    throw err;
  }

  this.comm = new commplugin(serviceDescription);
}

// export the class
module.exports = Client;