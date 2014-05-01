var PluginManager = require("./plugin-manager.njs");

Client.prototype.comm = null;
Client.prototype.spec = null;

function Client(serviceSpec) {
  "use strict";

  this.spec = serviceSpec;

  var commplugin;
  try {
    commplugin =  PluginManager.getClientPlugin(serviceSpec.type.name);
  } catch (err) {
    console.log("ERROR: An appropriate communication plugin could not be found for service at '" +
      serviceSpec.address + "'. To interact with this service the '" + serviceSpec.type.name +
      "' plugin is required.");
    console.log(err);
    throw err;
  }

  this.comm = new commplugin(serviceSpec);
}

// export the class
module.exports = Client;