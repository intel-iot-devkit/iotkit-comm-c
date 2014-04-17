'use strict';

/* 
 * initialize the edison library based on its configuration file.
 */
var path = require('path');

var edisonConfig = require("./config.js");
var InterfaceValidator = require("./core/plugin-validator.njs");

// init edison plugin validator
var validator = new InterfaceValidator();

// load component plugins
var component; // current component being processed

var essentialComponents = ["discovery", "communication"];

exports.serviceLib = require("./core/service-lib.js");

function setPluginAccessVariable(plugin) {
	if (!exports[component]) {
		exports[component] = {};
	}
	exports[component][plugin.prototype.type] = plugin;
}

//load specified plugins for all supported components
for (component in edisonConfig.components) {
	if (!edisonConfig.components[component] || !edisonConfig.components[component].plugins) {
		console.log("INFO: No plugins configured for component '" + component + "'. Skipping...");
		continue;
	}
	validator.validate(component, edisonConfig.components[component].plugins, setPluginAccessVariable);
}

//exports (no real need for this)
exports.config = edisonConfig;

// test function
exports.sayhello = function ()
{
	return "Hello Edison user!";
};

exports.getPlugin = function (component, type) {
	if (!exports[component]) {
		throw("Component '" + component + "' was not included in the edison library. " +
				"It is either not defined in the configuration file or has no plugins associated with it.");
	}
	
	if (!exports[component][type]) {
		throw("No plugin of type '" + type + "' exists for component '" + component + "'. " +
				"Please ensure that a plugin for this type is included in the configuration of this component.");
	}
	
	return exports[component][type];
};