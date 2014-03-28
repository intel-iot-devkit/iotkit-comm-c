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

function getPluginFilePath(pluginDescriptor) {
	// todo: will need to add code for non-core plugins at some point
	return path.join(edisonConfig.libRoot, edisonConfig.pluginDir, pluginDescriptor.fileName);
}

function getPluginInterfaceFilePath(component, type, core) {
	// todo: handle non-core plugins
	return path.join(edisonConfig.libRoot, edisonConfig.pluginInterfaceDir, component + "-" + type + ".json");
}
// Verifies that a plugin confirms to the interface type it claims to be.
function InterfaceValidator() {}

// methods
InterfaceValidator.prototype.validate = function(pluginList, callback)
{
	for (var i = 0; i < pluginList.length; i++) {
		// ignoring non-core plugins for now
		if (pluginList[i].ignored || !pluginList[i].core || !pluginList[i].fileName) {
			continue;
		}
		
		var pluginFilePath = getPluginFilePath(pluginList[i]);
		var plugin = require(pluginFilePath);
		
		var superInterfaceFilePath = path.join(edisonConfig.libRoot, edisonConfig.pluginInterfaceDir, edisonConfig.superInterfaceName + ".json");
		var superInterfaceSpec = JSON.parse(fs.readFileSync(superInterfaceFilePath));

		// check if interface contains properties and functions that all interfaces are required to have
		// need to do this here since plugin.component and plugin.type properties are needed below
		validateProperties(plugin, superInterfaceSpec, pluginFilePath);
		validateFunctions(plugin, superInterfaceSpec);

		var pluginInterfaceFilePath = getPluginInterfaceFilePath(plugin.component, plugin.type, pluginList[i].core);
		var pluginInterfaceSpec = JSON.parse(fs.readFileSync(pluginInterfaceFilePath));

		validateProperties(plugin, pluginInterfaceSpec);
		validateFunctions(plugin, pluginInterfaceSpec);
		
		callback(plugin);
	}
};

// needed to include like a class
module.exports = InterfaceValidator;
