var fs = require('fs');

ServiceDescriptionValidator.prototype.description = null;

function ServiceDescriptionValidator(serviceDescription) {
  "use strict";
  this.description = serviceDescription;
}

ServiceDescriptionValidator.prototype.readServiceDescriptionFromFile = function (serviceDescriptionFilePath) {
  "use strict";
  this.description = JSON.parse(fs.readFileSync(serviceDescriptionFilePath));
};

ServiceDescriptionValidator.prototype.readServiceDescriptionFromString = function (serviceDescriptionString) {
  "use strict";
  this.description = JSON.parse(serviceDescriptionString);
};

ServiceDescriptionValidator.validate = function (serviceDescription) {
  "use strict";
  if (typeof serviceDescription.name === 'undefined') {
    throw new Error("Service must have a name.");
  }

  if (!typeof serviceDescription.name === "string" || serviceDescription.name.length == 0) {
    throw new Error("Service name must be a non-zero string.");
  }

  if (typeof serviceDescription.type === 'undefined'
    || typeof serviceDescription.type.name === 'undefined'
    || typeof serviceDescription.type.protocol === 'undefined') {
    throw new Error("Service must have a type; a type.name; and a type.protocol field.");
  }

  if (typeof serviceDescription.type.subtypes !== 'undefined') {
    if (!Array.isArray(serviceDescription.type.subtypes)) {
      throw new Error("Subtypes of a service type must be in an array.");
    }

    if (serviceDescription.type.subtypes.length > 1) {
      throw new Error("More than one subtype is not supported at this time. This is a known issue.");
    }
  }

  if (typeof serviceDescription.port === 'undefined') {
    throw new Error("Service must have a port number.");
  }

  if (typeof serviceDescription.port !== 'number') {
    throw new Error("Service port must be a number.");
  }

  if (typeof serviceDescription.properties !== 'undefined') {
    if (typeof serviceDescription.properties !== 'object' || Array.isArray(serviceDescription.properties)) {
      throw new Error("Service properties must be in an object containing name/value pairs.");
    }
  }

  if (typeof serviceDescription.advertise !== 'undefined') {
    if (typeof serviceDescription.advertise.locally === 'undefined') {
      throw new Error("Missing boolean property 'advertise.locally. " +
        "Service needs to state if it must be locally advertised or not.");
    }
    if (typeof serviceDescription.advertise.locally !== 'boolean') {
      throw new Error("advertise.locally must be a Boolean property.");
    }

    if (typeof serviceDescription.advertise.cloud === 'undefined') {
      throw new Error("Missing boolean property 'advertise.cloud. " +
        "Service needs to state if it must be advertised in the cloud or not.");
    }

    if (typeof serviceDescription.advertise.locally !== 'boolean') {
      throw new Error("advertise.cloud must be a Boolean property.");
    }
  }

  if (typeof serviceDescription.address !== 'undefined') {
    if (typeof serviceDescription.address !== 'string') {
      throw new Error("Address must be a string. Currently, only IPv4 is supported.");
    }
  }

  if (typeof serviceDescription.comm_params !== 'undefined') {
    if (typeof serviceDescription.comm_params !== 'object') {
      throw new Error("Communication params field must be an object. It should contain name/value pairs.");
    }
  }

  return serviceDescription;
};

ServiceDescriptionValidator.prototype.getValidatedDescription = function () {
  "use strict";
  return ServiceDescriptionValidator.validate(this.description);
};

ServiceDescriptionValidator.validateRawServiceRecord = function (rawServiceRecord) {
  "use strict";
  ServiceDescriptionValidator.validate(rawServiceRecord);
};

module.exports = ServiceDescriptionValidator;
