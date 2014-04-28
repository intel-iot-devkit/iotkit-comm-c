var main = require("../main.njs");


Service.prototype.comm = null;
Service.prototype.spec = null;

function Service(serviceSpec) {
  "use strict";

  this.spec = serviceSpec;

  var commplugin;
  try {
    commplugin =  main.getServicePlugin(this.spec.type.name);
  } catch (err) {
    console.log("ERROR: An appropriate communication plugin could not be found for service '" + this.spec.name +
      "'. Service needs communication plugin '" + this.spec.type.name + "'.");
    throw err;
  }

  this.comm = new commplugin(serviceSpec);
}

// export the class
module.exports = Service;