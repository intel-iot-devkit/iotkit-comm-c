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
exports.localDirectory.advertiseServices(edisonConfig.libroot + '/' + edisonConfig.serviceDir);

// other components...

console.log("Edison is now initialized!");

//exports (no real need for this)
exports.sayhello = function ()
{
	return "Hello Edison user!";
};
