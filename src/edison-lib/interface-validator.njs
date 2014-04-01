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
			throw("Plugin '" + plugin.name + "' does not define '" + interfaceSpec.functions[i] + "' as a function.");
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

// format: { "type": ["plugin1", "plugin2"]}
InterfaceValidator.prototype.loadedPlugins = {};

// methods

//Verifies that a plugin confirms to the interface type it claims to be.
InterfaceValidator.prototype.validate = function(component, pluginList, callback)
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
		
		if (component !== plugin.component) {
			throw ("Plugin '" + plugin.name + "' was not written for component '" + component +
					"'. Please edit config file and make this plugin load for component '" + plugin.component + "'.");
		}
		
		var pluginInterfaceFilePath = getPluginInterfaceFilePath(plugin.component, plugin.type, pluginList[i].core);
		var pluginInterfaceSpec = JSON.parse(fs.readFileSync(pluginInterfaceFilePath));

		validateProperties(plugin, pluginInterfaceSpec);
		validateFunctions(plugin, pluginInterfaceSpec);
		
		if (!this.loadedPlugins[plugin.component]) {
			this.loadedPlugins[plugin.component] = {};
		}
		
		if (!this.loadedPlugins[plugin.component][plugin.type]) {	
			this.loadedPlugins[plugin.component][plugin.type] = [];
		} else {
			if (this.loadedPlugins[plugin.component][plugin.type].indexOf(plugin.name) != -1) {
				console.log("INFO: Plugin '" + plugin.name + "' for component '" + plugin.component + "' already exists. Skipping...");
				continue;
			}
			
			console.log("WARNING: Following plugins with type '" + plugin.type +
					"' already exist for component '" + plugin.component + "'");
			console.log("(" + this.loadedPlugins[plugin.component][plugin.type] + ")");
			console.log("The plugin that was just loaded is " + plugin.name +
					", and it is the new active plugin for component '" + plugin.component + 
					"' and type '" + plugin.type +"'");	
		}
		this.loadedPlugins[plugin.component][plugin.type].push(plugin.name);
		
		callback(plugin);
	}
};

function InterfaceValidator() {
	
}

// needed to include like a class
module.exports = InterfaceValidator;
