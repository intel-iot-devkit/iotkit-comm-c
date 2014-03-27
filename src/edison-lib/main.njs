'use strict';

// initialize the edison library
var edisonConfig = require("./config.js").config;
var InterfaceValidator = require("./interface-validator.njs");

// load plugin for local discovery
var validator = new InterfaceValidator(edisonConfig.libroot + "/" + edisonConfig.interfaceImplDir + "/" + edisonConfig.localDiscovery);
validator.validate();
console.log("Validated discovery plugin.");

// exports (keep this small for this file)
exports.sayhello = function ()
{
	return "Hello Edison user!";
};
