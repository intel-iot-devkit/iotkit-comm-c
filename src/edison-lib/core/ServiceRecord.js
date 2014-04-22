var ServiceDescriptionValidator = require("./ServiceDescriptionValidator.js");

// private static
exports.COMM_PARAMS_PREFIX = "_comm_params_";

ServiceRecord.prototype.rawRecord = {};
ServiceRecord.prototype.serviceDescription = {};
ServiceRecord.prototype.suggestedAddresses = [];

function ServiceRecord (serviceDescription) {
  "use strict";
  this.serviceDescription = serviceDescription;
  makeServiceRecord.call(this);
}

ServiceRecord.prototype.getSuggestedServiceDescription = function () {
  "use strict";

  if (this.serviceDescription.address) {
    return this.serviceDescription;
  }

  this.serviceDescription.address = this.getSuggestedAddress();
  if (!this.serviceDescription.address) {
    throw new Error("No valid address found for this service.");
  }

  return this.serviceDescription;
};

ServiceRecord.prototype.getSuggestedAddress = function () {
  "use strict";

  if (this.serviceDescription.address) {
    return this.serviceDescription.address;
  }

  if (this.suggestedAddresses.length != 0) {
    return this.suggestedAddresses[0];
  }

  if (this.rawRecord.addresses.length != 0) {
    return this.rawRecord.addresses[0];
  }

  return "";
};

ServiceRecord.prototype.setSuggestedAddress = function (address) {
  "use strict";
  this.serviceDescription.address = address;
};

ServiceRecord.prototype.getSuggestedAddresses = function (addresses) {
  "use strict";
  return this.suggestedAddresses;
};

ServiceRecord.prototype.setSuggestedAddresses = function (addresses) {
  "use strict";
  this.suggestedAddresses = addresses;
};

ServiceRecord.prototype.initFromRawServiceRecord = function (serviceRecord) {
  "use strict";

  ServiceDescriptionValidator.validateRawServiceRecord(serviceRecord);

  this.rawRecord = serviceRecord;
  this.serviceDescription = serviceRecord;

  if (this.rawRecord.addresses && this.rawRecord.addresses.length > 0) {
    this.suggestedAddresses = this.rawRecord.addresses;
    this.serviceDescription.address = this.rawRecord.addresses[0];
  } else {
    this.serviceDescription.address = "";
  }

  if (typeof this.rawRecord.properties === 'undefined') {
    return;
  }

  if (typeof this.serviceDescription.comm_params === 'undefined') {
    this.serviceDescription.comm_params = {};
  }

  var properties = Object.keys(this.rawRecord.properties);
  for (var i = 0; i < properties.length; i++) {
    if (properties[i].indexOf(exports.COMM_PARAMS_PREFIX) != 0) {
      continue;
    }

    var comm_param_key = properties[i].substring(exports.COMM_PARAMS_PREFIX.length);
    var comm_param_value = this.rawRecord.properties[properties[i]];

    this.serviceDescription.comm_params[comm_param_key] = comm_param_value;
  }
};

function makeServiceRecord () {
  "use strict";

  this.rawRecord = this.serviceDescription;

  if (!this.serviceDescription) {
    return;
  }

  if (!this.serviceDescription.comm_params) {
    return;
  }

  if (typeof this.rawRecord.properties === 'undefined') {
    this.rawRecord.properties = {};
  }

  var commparams = Object.keys(this.serviceDescription.comm_params);
  for (var i = 0; i < commparams.length; i++) {
    var propertyName = exports.COMM_PARAMS_PREFIX + commparams[i];
    if (this.rawRecord.properties[propertyName]) {
      continue;
    }
    this.rawRecord.properties[propertyName] = this.serviceDescription.comm_params[commparams[i]];
  }
}

module.exports = ServiceRecord;