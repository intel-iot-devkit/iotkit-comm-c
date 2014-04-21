var main = require("../main.njs");
var ServiceDescriptionValidator = require("./ServiceDescriptionValidator.js");

function Service(serviceSpecFilePath) {
  "use strict";

  var validator = new ServiceDescriptionValidator(serviceSpecFilePath);
  this.description = validator.validate();

  var commplugin;
  try {
    commplugin =  main.getPlugin("communication", this.description.type.name);
  } catch (err) {
    console.log("ERROR: An appropriate communication plugin could not be found for service '" + this.description.name +
      "'. Service needs communication plugin '" + this.description.type.name + "'.");
    throw err;
  }

  this.comm = new commplugin(this.description.port, "server");
}

Service.prototype.comm = null;
Service.prototype.description = null;

// export the class
module.exports = Service;