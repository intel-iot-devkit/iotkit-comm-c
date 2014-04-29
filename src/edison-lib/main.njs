'use strict';

/* 
 * initialize the edison library based on its configuration file.
 */
var path = require('path');
var PluginLoader = require("./core/plugin-loader.njs");

var edisonConfig = require("./config.js");
var ServiceSpecValidator = require("./core/ServiceSpecValidator.js");
var ServiceQuery = require("./core/ServiceQuery.js");
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

exports.ServiceSpecValidator = ServiceSpecValidator;

exports.ServiceQuery = ServiceQuery;

exports.sayhello = function ()
{
	return "Hello Edison user!";
};

exports.getClientPlugin = function (name) {
	if (!exports.plugins[name]) {
		throw new Error("No plugin with name '" + name + "' exists." +
				"Please ensure that this plugin is included in the configuration file.");
	}
	
	return exports.plugins[name][edisonConfig.communication.pluginFileSuffixes.clientFileSuffix];
};

exports.getServicePlugin = function (name) {
  if (!exports.plugins[name]) {
    throw new Error("No plugin with name '" + name + "' exists." +
      "Please ensure that this plugin is included in the configuration file.");
  }

  return exports.plugins[name][edisonConfig.communication.pluginFileSuffixes.serverFileSuffix];
};

exports.createService = function (serviceSpec, serviceCreatedCallback) {
  var service = new Service(serviceSpec);
  if (!service.spec.advertise || service.spec.advertise.locally) {
    EdisonMDNS.advertiseService(service.spec);
  }
  serviceCreatedCallback(service);
}

exports.createClient = function (serviceQuery, serviceFilter, clientCreatedCallback) {
  EdisonMDNS.discoverServices(serviceQuery, serviceFilter, function(serviceSpec) {
    clientCreatedCallback(new Client(serviceSpec));
  });
}

exports.discoverServices = function (serviceQuery, serviceFoundCallback) {
  EdisonMDNS.discoverServices(serviceQuery, null, serviceFoundCallback);
};

exports.createClientForGivenService = function (serviceSpec, clientCreatedCallback) {
  clientCreatedCallback(new Client(serviceSpec));
};