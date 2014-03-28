var edisonConfig = require("./config.js");
var path = require('path');
var fs = require('fs');
		
// functions in local scope

// if pluginFilePath is not supplied, it is assumed that property plugin.name exists
function validateProperties(plugin, interfaceSpec, pluginFilePath) {
	if(!interfaceSpec.properties) {
		return;
	}
	
	// check if all required properties exist
	for (var i in interfaceSpec.properties) {
		if (!plugin[interfaceSpec.properties[i]]) {
			if(pluginFilePath) {
				throw("Plugin at '" + pluginFilePath + "' does not define required property '" + interfaceSpec.properties[i] + "'.");
			} else {
				throw("Plugin '" + plugin.name + "' does not define required property '" + interfaceSpec.properties[i] + "'.");
			}
		}
	}
}

function validateFunctions(plugin, interfaceSpec) {
	if(!interfaceSpec.functions) {
		return;
	}
	
	// check if all required function names exist
	for (var j in interfaceSpec.functions) {
		if (!plugin[interfaceSpec.functions[j]]) {
			throw("Plugin '" + plugin.name + "' does not define required function '" + interfaceSpec.functions[j] + "'.");
		}
	}
	
	// check that type is indeed a function
	for (var i in interfaceSpec.functions) {
		if ((typeof plugin[interfaceSpec.functions[i]]) !== "function") {
			throw("Plugin '" + plugin.name + "' does not define required function '" + interfaceSpec.functions[i] + "'.");
		}
	}
}

// Verifies that a plugin confirms to the interface type it claims to be.
function InterfaceValidator() {}

// vars
//InterfaceValidator.prototype.interfaceSpec = {}; // JSON object
//InterfaceValidator.prototype.interfaceInstance = null; // Object
//InterfaceValidator.prototype.pluginPath = "";

// methods
InterfaceValidator.prototype.validate = function(pluginFilePath)
{
	var plugin = require(pluginFilePath);
	var superInterfaceFilePath = path.join(edisonConfig.libRoot, edisonConfig.interfaceDir, edisonConfig.superInterfaceName + ".json");
	var superInterfaceSpec = JSON.parse(fs.readFileSync(superInterfaceFilePath));
	
	// check if interface contains properties and functions that all interfaces are required to have 
	validateProperties(plugin, superInterfaceSpec, pluginFilePath);
	validateFunctions(plugin, superInterfaceSpec);
	
	// read the correct interface spec to compare our plugin against
	if (!edisonConfig.components[plugin.component]) {
		// we know plugin.component exists (see validateProperties() call above)
		throw("Could not find plugin interface specification for component '" + plugin.component + "'.");
	}
	
	var interfaceFilePath = path.join(edisonConfig.libRoot, edisonConfig.interfaceDir, plugin.component + ".json");
	var interfaceSpec = JSON.parse(fs.readFileSync(interfaceFilePath));
	
	validateProperties(plugin, interfaceSpec);
	validateFunctions(plugin, interfaceSpec);
	
	return plugin;
};

// needed to include like a class
module.exports = InterfaceValidator;
