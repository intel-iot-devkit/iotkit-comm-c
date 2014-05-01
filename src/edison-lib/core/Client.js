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
    console.log("ERROR: Could not load communication plugin needed to interact with service at '" +
      serviceSpec.address + ":" + serviceSpec.port + "'. Plugin '" + serviceSpec.type.name + "' was not found or produced errors while loading.");
    console.log(err);
    return;
  }

  this.comm = new commplugin(serviceSpec);
}

// export the class
module.exports = Client;