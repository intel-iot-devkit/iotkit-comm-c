'use strict';

var ConfigManager = require("./core/config-manager.js");
ConfigManager.init(__dirname, "config.json");

exports.config = ConfigManager.config;
exports.ServiceSpecValidator = require("./core/ServiceSpecValidator.js");
exports.ServiceQuery = require("./core/ServiceQuery.js");

exports.sayhello = function ()
{
	return "Hello Edison user!";
};

var Service = require("./core/Service.js");
var Client = require("./core/Client.js");
var EdisonMDNS = require("./core/EdisonMDNS.js"); // singleton use as is

exports.createService = function (serviceSpec, serviceCreatedCallback) {
  var service = new Service(serviceSpec);
  if (!service.spec.advertise || service.spec.advertise.locally) {
    EdisonMDNS.advertiseService(service.spec);
  }
  serviceCreatedCallback(service);
};

exports.createClient = function (serviceQuery, serviceFilter, clientCreatedCallback) {
  EdisonMDNS.discoverServices(serviceQuery, serviceFilter, function(serviceSpec) {
    clientCreatedCallback(new Client(serviceSpec));
  });
};

exports.discoverServices = function (serviceQuery, serviceFoundCallback) {
  EdisonMDNS.discoverServices(serviceQuery, null, serviceFoundCallback);
};

exports.createClientForGivenService = function (serviceSpec, clientCreatedCallback) {
  clientCreatedCallback(new Client(serviceSpec));
};