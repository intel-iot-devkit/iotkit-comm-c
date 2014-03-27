'use strict';

/* 
 * initialize the edison library based on its configuration file.
 */

var edisonConfig = require("./config.js").config;
var InterfaceValidator = require("./interface-validator.njs");

// load plugin for local discovery
var validator = new InterfaceValidator();
validator.validate(edisonConfig.libroot + "/" + edisonConfig.interfaceImplDir + "/" + edisonConfig.localDiscovery);
console.log("Validated discovery plugin.");

// exports (keep this small for this file)
exports.sayhello = function ()
{
	return "Hello Edison user!";
};
