'use strict';

/* 
 * initialize the edison library based on its configuration file.
 */
var path = require('path');
var PluginLoader = require("./core/plugin-loader.njs");

var edisonConfig = require("./config.js");
var ServiceDescriptionValidator = require("./core/ServiceDescriptionValidator.js");
var Service = require("./core/Service.js");
var Client = require("./core/Client.js");
var EdisonMDNS = require("./core/EdisonMDNS.js"); // singleton use as is

//load specified plugins for all supported components
var pluginLoader = new PluginLoader();
if (!edisonConfig.communication.plugins || edisonConfig.communication.plugins.length == 0) {
  throw new Error("At least one communication plugin must be configured.");
}

for (var i = 0; i < edisonConfig.communication.plugins.length; i++) {
  if (edisonConfig.communication.plugins[i].ignored || !edisonConfig.communication.plugins[i].pluginName) {
    continue;
  }
  pluginLoader.loadPlugin(edisonConfig.communication.plugins[i].pluginName);
}

exports.plugins = pluginLoader.loadedPlugins;

exports.config = edisonConfig;

exports.ServiceDescriptionValidator = ServiceDescriptionValidator;

exports.sayhello = function ()
{
	return "Hello Edison user!";
};

exports.getClientPlugin = function (name) {
	if (!exports.plugins[name]) {
		throw new Error("No plugin with name '" + name + "' exists." +
				"Please ensure that this plugin is included in the configuration file.");
	}
	
	return exports.plugins[name][edisonConfig.pluginFileSuffixes.clientFileSuffix];
};

exports.getServicePlugin = function (name) {
  if (!exports.plugins[name]) {
    throw new Error("No plugin with name '" + name + "' exists." +
      "Please ensure that this plugin is included in the configuration file.");
  }

  return exports.plugins[name][edisonConfig.pluginFileSuffixes.serverFileSuffix];
};

exports.createService = function (serviceDescription, serviceCreatedCallback) {
  var service = new Service(serviceDescription);
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