'use strict';

/* 
 * initialize the edison library based on its configuration file.
 */
var path = require('path');
var InterfaceValidator = require("./core/plugin-validator.njs");

var edisonConfig = require("./config.js");
var ServiceDescriptionValidator = require("./core/ServiceDescriptionValidator.js");
var Service = require("./core/Service.js");
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
	validator.getValidatedDescription(component, edisonConfig.components[component].plugins, setPluginAccessVariable);
}

exports.config = edisonConfig;

exports.ServiceDescriptionValidator = ServiceDescriptionValidator;

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

exports.createService = function (serviceDescription, serviceCreatedCallback) {
  var service = new Service(serviceDescription)
  if (!service.description.advertise || service.description.advertise.locally) {
    EdisonMDNS.advertiseService(service.description);
  }
  serviceCreatedCallback(service);
}

exports.createClient = function (serviceQuery, serviceFilter, clientCreatedCallback) {
  EdisonMDNS.discoverServices(serviceQuery, serviceFilter, function(serviceDescription) {
    clientCreatedCallback(new Client(serviceDescription));
  });
}

exports.discoverServices = function (serviceQuery, serviceFoundCallback) {
  EdisonMDNS.discoverServices(serviceQuery, null, serviceFoundCallback);
};

exports.createClientForGivenService = function (serviceDescription, clientCreatedCallback) {
  clientCreatedCallback(new Client(serviceDescription));
};