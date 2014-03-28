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
var localDirectory = validator.validate(localDirectoryPluginPath);

// other components...
console.log("Edison is now initialized!");


//exports (no real need for this)
exports.config = edisonConfig;
exports.localDirectory = localDirectory;
exports.sayhello = function ()
{
	return "Hello Edison user!";
};
