var fs = require('fs');

var Service = require("./Service.js");

exports.discovery = null;

function validateServiceDescription(description) {
  if (typeof description.name === 'undefined') {
    throw new Error("Service must have a name.");
  }

  if (!typeof description.name === "string" || description.name.length == 0) {
    throw new Error("Service name must be a non-zero string.");
  }

  if (typeof description.type === 'undefined'
    || typeof description.type.name === 'undefined'
    || typeof description.type.protocol === 'undefined') {
    throw new Error("Service must have a type; a type.name; and a type.protocol field.");
  }

  if (typeof description.type.subtypes !== 'undefined') {
    if (!Array.isArray(description.type.subtypes)
      || description.type.subtypes.length == 0) {
      throw new Error("Subtypes of a service type must be in an array of non-zero length.");
    }
  }

  if (typeof description.port === 'undefined') {
    throw new Error("Service must have a port number.");
  }

  if (typeof description.port !== 'number') {
    throw new Error("Service port must be a number.");
  }

  if (typeof description.properties !== 'undefined') {
    if (typeof description.properties !== 'object' || Array.isArray(description.properties)) {
      throw new Error("Service properties must be in an object containing name/value pairs.");
    }
  }
}

exports.createServiceFromString = function(jsonDescription) {
  "use strict";
  if (!typeof jsonDescription === "string") {
    throw new Error("Unsupported service description object.");
  }
  var descriptionObj = JSON.parse(jsonDescription);
  validateServiceDescription(descriptionObj);
  return new Service(descriptionObj);
};

exports.createService = function (filePath) {
  "use strict";
  var descriptionObj = JSON.parse(fs.readFileSync(filePath));
  validateServiceDescription(descriptionObj);
  return new Service(descriptionObj);
};