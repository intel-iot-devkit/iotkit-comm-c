'use strict';

/* 
 * initialize the edison library based on its configuration file.
 */
var path = require('path');

var edisonConfig = require("./config.js");
var InterfaceValidator = require("./interface-validator.njs");

// init edison plugin validator
var validator = new InterfaceValidator();

// load plugins

// local directory component (for discovery etc.)
var localDirectoryPluginPath = path.join(edisonConfig.libRoot, edisonConfig.interfaceImplDir, edisonConfig.components.localDirectory);
exports.localDirectory = validator.validate(localDirectoryPluginPath);
exports.localDirectory.advertiseServices(path.join(edisonConfig.libRoot, edisonConfig.serviceDir));

// other components...
console.log("Edison is now initialized!");


//exports (no real need for this)
exports.sayhello = function ()
{
	return "Hello Edison user!";
};
