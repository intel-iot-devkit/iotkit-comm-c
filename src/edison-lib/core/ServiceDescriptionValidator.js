var fs = require('fs');

ServiceDescriptionValidator.prototype.description = null;

ServiceDescriptionValidator.prototype.validate = function () {
  "use strict";

  if (typeof this.description.name === 'undefined') {
    throw new Error("Service must have a name.");
  }

  if (!typeof this.description.name === "string" || this.description.name.length == 0) {
    throw new Error("Service name must be a non-zero string.");
  }

  if (typeof this.description.type === 'undefined'
    || typeof this.description.type.name === 'undefined'
    || typeof this.description.type.protocol === 'undefined') {
    throw new Error("Service must have a type; a type.name; and a type.protocol field.");
  }

  if (typeof this.description.type.subtypes !== 'undefined') {
    if (!Array.isArray(this.description.type.subtypes)
      || this.description.type.subtypes.length == 0) {
      throw new Error("Subtypes of a service type must be in an array of non-zero length.");
    }
  }

  if (typeof this.description.port === 'undefined') {
    throw new Error("Service must have a port number.");
  }

  if (typeof this.description.port !== 'number') {
    throw new Error("Service port must be a number.");
  }

  if (typeof this.description.properties !== 'undefined') {
    if (typeof this.description.properties !== 'object' || Array.isArray(this.description.properties)) {
      throw new Error("Service properties must be in an object containing name/value pairs.");
    }
  }

  if (typeof this.description.advertise === 'undefined') {
    throw new Error("Service must specify if it needs to advertised locally or in the cloud.");
  }

  if (typeof this.description.advertise.locally === 'undefined') {
    throw new Error("Missing boolean property 'advertise.locally. " +
      "Service needs to state if it must be locally advertised or not.");
  }
  if (typeof this.description.advertise.locally !== 'boolean') {
    throw new Error("advertise.locally must be a Boolean property.");
  }

  if (typeof this.description.advertise.cloud === 'undefined') {
    throw new Error("Missing boolean property 'advertise.cloud. " +
      "Service needs to state if it must be advertised in the cloud or not.");
  }
  if (typeof this.description.advertise.locally !== 'boolean') {
    throw new Error("advertise.cloud must be a Boolean property.");
  }

  return this.description;
};

function ServiceDescriptionValidator(serviceSpecFilePath) {
  "use strict";
  this.description = JSON.parse(fs.readFileSync(serviceSpecFilePath));
}

module.exports = ServiceDescriptionValidator;
