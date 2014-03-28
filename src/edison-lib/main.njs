'use strict';

/* 
 * initialize the edison library based on its configuration file.
 */

var edisonConfig = require("./config.js").config;
var InterfaceValidator = require("./interface-validator.njs");

// init edison plugin validator
var validator = new InterfaceValidator();

// load plugins

// local discovery component
exports.localDirectory = validator.validate(edisonConfig.libroot + "/" + edisonConfig.interfaceImplDir + "/" + edisonConfig.components.localDirectory);

// other components...

console.log("Edison is now initialized!");

// exports (keep this small for this file)
exports.sayhello = function ()
{
	return "Hello Edison user!";
};