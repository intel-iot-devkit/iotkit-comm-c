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

// load specified plugins for all supported components
function setPluginAccessVariable(plugin) {
	if (!exports[plugin.component]) {
		exports[plugin.component] = {};
	}
	exports[plugin.component][plugin.type] = plugin;
}

for (var component in edisonConfig.components) {
	if (!edisonConfig.components[component] || !edisonConfig.components[component].plugins) {
		console.log("INFO: No plugins configured for component '" + component + "'. Skipping...");
		continue;
	}
	validator.validate(component, edisonConfig.components[component].plugins, setPluginAccessVariable);
}

console.log("Edison is now initialized!");

//exports (no real need for this)
exports.config = edisonConfig;

// test function
exports.sayhello = function ()
{
	return "Hello Edison user!";
};
