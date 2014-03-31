'use strict';

/* 
 * initialize the edison library based on its configuration file.
 */
var path = require('path');

var edisonConfig = require("./config.js");
var InterfaceValidator = require("./interface-validator.njs");

// init edison plugin validator
var validator = new InterfaceValidator();

// load component plugins

// supported components
var localDirectory = {};
var localComm = {};

// local directory plugins (for discovery etc.)
validator.validate(edisonConfig.components.localDirectory.plugins, function (plugin) {
	localDirectory[plugin.type] = plugin;
});

// local communication plugins
validator.validate(edisonConfig.components.localComm.plugins, function (plugin) {
	localComm[plugin.type] = plugin;
});

// load other component plugins ...

console.log("Edison is now initialized!");

//exports (no real need for this)
exports.config = edisonConfig;
exports.localDirectory = localDirectory;
exports.localComm = localComm;

// test function
exports.sayhello = function ()
{
	return "Hello Edison user!";
};
