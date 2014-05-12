var ServiceSpecValidator = require("./ServiceSpecValidator.js");

// private static
exports.COMM_PARAMS_PREFIX = "_comm_params_";

ServiceRecord.prototype.rawRecord = {};
ServiceRecord.prototype.spec = {};
ServiceRecord.prototype.suggestedAddresses = [];

function ServiceRecord (serviceSpec) {
  "use strict";
  this.spec = serviceSpec;
  makeServiceRecord.call(this);
}

ServiceRecord.prototype.getSuggestedServiceSpec = function () {
  "use strict";

  if (this.spec.address) {
    return this.spec;
  }

  this.spec.address = this.getSuggestedAddress();
  if (!this.spec.address) {
    throw new Error("No valid address found for this service.");
  }

  return this.spec;
};

ServiceRecord.prototype.getSuggestedAddress = function () {
  "use strict";

  if (this.spec.address) {
    return this.spec.address;
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
  this.spec.address = address;
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

  ServiceSpecValidator.validateRawServiceRecord(serviceRecord);

  this.rawRecord = serviceRecord;
  this.spec = serviceRecord;

  if (this.rawRecord.addresses && this.rawRecord.addresses.length > 0) {
    this.suggestedAddresses = this.rawRecord.addresses;
    this.spec.address = this.rawRecord.addresses[0];
  } else {
    this.spec.address = "";
  }

  if (typeof this.rawRecord.properties === 'undefined') {
    return;
  }

  if (typeof this.spec.comm_params === 'undefined') {
    this.spec.comm_params = {};
  }

  var properties = Object.keys(this.rawRecord.properties);
  for (var i = 0; i < properties.length; i++) {
    if (properties[i].indexOf(exports.COMM_PARAMS_PREFIX) != 0) {
      continue;
    }

    var comm_param_key = properties[i].substring(exports.COMM_PARAMS_PREFIX.length);
    var comm_param_value = this.rawRecord.properties[properties[i]];

    this.spec.comm_params[comm_param_key] = comm_param_value;
  }
};

function makeServiceRecord () {
  "use strict";

  this.rawRecord = this.spec;

  if (!this.spec) {
    return;
  }

  if (!this.spec.comm_params) {
    return;
  }

  if (typeof this.rawRecord.properties === 'undefined') {
    this.rawRecord.properties = {};
  }

  var commparams = Object.keys(this.spec.comm_params);
  for (var i = 0; i < commparams.length; i++) {
    var propertyName = exports.COMM_PARAMS_PREFIX + commparams[i];
    if (this.rawRecord.properties[propertyName]) {
      continue;
    }
    this.rawRecord.properties[propertyName] = this.spec.comm_params[commparams[i]];
  }
}

module.exports = ServiceRecord;