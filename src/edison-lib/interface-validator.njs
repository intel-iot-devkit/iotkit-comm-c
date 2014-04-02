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
			if (this.assumedPrototypeExists) {
				console.log("WARNING: Since module.exports was set to a function, it is assumed that the " +
						"plugin is a class defined using the prototype object. If this is not the case, " +
						"please make sure to use the prototype object, or the exports.<function name>, " +
						"or module.exports = {<all your functions>} convention to export functions in your plugin.");
			}
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
	return path.join(edisonConfig.libRoot, edisonConfig.pluginDir, pluginDescriptor.fileName);
}

function getPluginInterfaceFilePath(pluginComponents, type, configComponent) {
	var possibleComponents = [];

	// for...in loops should not be used for arrays in javascript.
	// don't know why, but got 0s instead of array contents
	for (var i = 0; i < pluginComponents.length; ++i) {
		var currentPath = path.join(edisonConfig.libRoot,
				edisonConfig.pluginInterfaceDir,
				pluginComponents[i] + "-" + type + ".json");
		
		if (fs.existsSync(currentPath))
		{ // interface file exists for this component and plugin type
			possibleComponents.push(pluginComponents[i]);
		}
	}
	
	if (possibleComponents.length === 0) {
		throw ("No interface specification file found for any of the following components '" +
				pluginComponents + "' and type '" + type + "'.");
	}
	
	if (possibleComponents.indexOf(configComponent) !== -1)
	{ // currently processed component has a valid interface spec for the plugin type, use it
		return path.join(edisonConfig.libRoot, edisonConfig.pluginInterfaceDir,
				configComponent + "-" + type + ".json");
	}

	// otherwise, just return the one that exists
	return path.join(edisonConfig.libRoot, edisonConfig.pluginInterfaceDir,
			possibleComponents[0] + "-" + type + ".json");
}

// format: { "type": ["plugin1", "plugin2"]}
InterfaceValidator.prototype.loadedPlugins = {};
InterfaceValidator.prototype.assumedPrototypeExists = false;

// methods

//Verifies that a plugin confirms to the interface type it claims to be.
InterfaceValidator.prototype.validate = function(component, pluginList, callback)
{
	for (var i = 0; i < pluginList.length; i++) {
		if (pluginList[i].ignored || !pluginList[i].fileName) {
			continue;
		}
		
		var pluginFilePath = getPluginFilePath(pluginList[i]);
		var plugin = require(pluginFilePath);
		
		if (typeof plugin === "function")
		{ // if plugin is defined as a class with module.exports = <constructor name>
			if(plugin.prototype) {
				plugin = plugin.prototype;
				this.assumedPrototypeExists = true;
			} else {
				throw ("Plugin at " + pluginFilePath + "must be defined using the prototype object if it is a class." +
						" If not, please use the exports.<function name> or module.exports = {<all your functions>}" +
						" method to export functions in your plugin.");
			}
		}
		
		var superInterfaceFilePath = path.join(edisonConfig.libRoot, edisonConfig.pluginInterfaceDir, edisonConfig.superInterfaceName + ".json");
		var superInterfaceSpec = JSON.parse(fs.readFileSync(superInterfaceFilePath));

		// check if interface contains properties and functions that all interfaces are required to have
		// need to do this here since plugin.component and plugin.type properties are needed below
		validateProperties(plugin, superInterfaceSpec, pluginFilePath);
		validateFunctions(plugin, superInterfaceSpec);
		
		if (!Array.isArray(plugin.components)) {
			throw ("Property 'components' in plugin '" + plugin.name +
					"' must be an array of valid component names.");
		}
		
		if (plugin.components.indexOf(component) === -1) {
			throw ("Plugin '" + plugin.name + "' was not written for component '" + component +
					"'. Please edit config file and make this plugin load for one of the following components '" +
					plugin.components + "'.");
		}
		
		var pluginInterfaceFilePath = getPluginInterfaceFilePath(plugin.components, plugin.type, component);
		var pluginInterfaceSpec = JSON.parse(fs.readFileSync(pluginInterfaceFilePath));

		validateProperties(plugin, pluginInterfaceSpec);
		validateFunctions(plugin, pluginInterfaceSpec);
		
		
		
		if (!this.loadedPlugins[component]) {
			this.loadedPlugins[component] = {};
		}
		
		if (!this.loadedPlugins[component][plugin.type]) {	
			this.loadedPlugins[component][plugin.type] = [];
		} else {
			if (this.loadedPlugins[component][plugin.type].indexOf(plugin.name) !== -1) {
				console.log("INFO: Plugin '" + plugin.name + "' for component '" + component + "' already exists. Skipping...");
				continue;
			}
			
			console.log("WARNING: Following plugins with type '" + plugin.type +
					"' already exist for component '" + component + "'");
			console.log("(" + this.loadedPlugins[component][plugin.type] + ")");
			console.log("The plugin that was just loaded is " + plugin.name +
					", and it is the new active plugin for component '" + component + 
					"' and type '" + plugin.type +"'");	
		}
		this.loadedPlugins[component][plugin.type].push(plugin.name);
		
		callback(plugin);
	}
};

function InterfaceValidator() {
	
}

// needed to include like a class
module.exports = InterfaceValidator;
