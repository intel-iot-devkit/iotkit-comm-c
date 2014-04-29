'use strict';

/* 
 * initialize the edison library based on its configuration file.
 */
var PluginLoader = require("./core/plugin-loader.njs");
var edisonConfig = require("./config.js");
var ServiceSpecValidator = require("./core/ServiceSpecValidator.js");
var ServiceQuery = require("./core/ServiceQuery.js");
var Service = require("./core/Service.js");
var Client = require("./core/Client.js");
var EdisonMDNS = require("./core/EdisonMDNS.js"); // singleton use as is

var pluginLoader = new PluginLoader();
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
    pluginLoader.loadPlugin(name);
	}

	return exports.plugins[name][edisonConfig.communicationPlugins.fileSuffixes.clientFileSuffix];
};

exports.getServicePlugin = function (name) {
  if (!exports.plugins[name]) {
    pluginLoader.loadPlugin(name);
  }

  return exports.plugins[name][edisonConfig.communicationPlugins.fileSuffixes.serverFileSuffix];
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