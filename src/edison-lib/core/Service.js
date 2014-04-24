var main = require("../main.njs");


Service.prototype.comm = null;
Service.prototype.description = null;

function Service(serviceDescription) {
  "use strict";

  this.description = serviceDescription;

  var commplugin;
  try {
    commplugin =  main.getServicePlugin(this.description.type.name);
  } catch (err) {
    console.log("ERROR: An appropriate communication plugin could not be found for service '" + this.description.name +
      "'. Service needs communication plugin '" + this.description.type.name + "'.");
    throw err;
  }

  this.comm = new commplugin(serviceDescription);
}

// export the class
module.exports = Service;