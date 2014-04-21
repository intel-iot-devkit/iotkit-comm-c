'use strict';

/* 
 * initialize the edison library based on its configuration file.
 */
var path = require('path');
var InterfaceValidator = require("./core/plugin-validator.njs");

var edisonConfig = require("./config.js");
var Service = require("./core/Service.js");
var ServiceDescriptionValidator = require("./core/ServiceDescriptionValidator.js");
var Client = require("./core/Client.js");
var EdisonMDNS = require("./core/EdisonMDNS.js"); // singleton use as is

function setPluginAccessVariable(component, plugin) {
  if (!exports.plugins) {
    exports.plugins = {};
  }

	if (!exports.plugins[component]) {
		exports.plugins[component] = {};
	}
	exports.plugins[component][plugin.prototype.name] = plugin;
}

//load specified plugins for all supported components
var validator = new InterfaceValidator();
for (var component in edisonConfig.components) {
	if (!edisonConfig.components[component] || !edisonConfig.components[component].plugins) {
		console.log("INFO: No plugins configured for component '" + component + "'. Skipping...");
		continue;
	}
	validator.validate(component, edisonConfig.components[component].plugins, setPluginAccessVariable);
}

exports.config = edisonConfig;

exports.Service = require("./core/Service.js");

exports.sayhello = function ()
{
	return "Hello Edison user!";
};

exports.getPlugin = function (component, name) {
	if (!exports.plugins[component]) {
		throw new Error("Component '" + component + "' was not included in the edison library. " +
				"It is either not defined in the configuration file or has no plugins associated with it.");
	}
	
	if (!exports.plugins[component][name]) {
		throw new Error("No plugin with name '" + name + "' exists for component '" + component + "'. " +
				"Please ensure that this plugin is included in the configuration of the respective component.");
	}
	
	return exports.plugins[component][name];
};

exports.createService = function (serviceSpecFilePath, serviceCreatedCallback) {
  var service = new Service(serviceSpecFilePath)
  if (service.description.advertise.locally) {
    EdisonMDNS.advertiseService(service.description);
  }
  serviceCreatedCallback(service);
}

exports.createClient = function (serviceQueryFilePath, serviceFilter, clientCreatedCallback) {
  // todo: change to ServiceQueryValidator
  var validator = new ServiceDescriptionValidator(serviceQueryFilePath);
  var serviceQuery = validator.validate();
  EdisonMDNS.discoverServices(serviceQuery.type, serviceFilter, function(serviceRecord, bestKnownAddress) {
    clientCreatedCallback(new Client(serviceQuery, bestKnownAddress, serviceRecord.port));
  });
}