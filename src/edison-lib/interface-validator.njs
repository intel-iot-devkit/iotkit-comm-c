var edisonConfig = require("./config.js");
var path = require('path');
var fs = require('fs');
		
// private functions and variables
var assumedPrototypeExists;

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
			if (assumedPrototypeExists) {
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
		
		if (typeof plugin === "function" && plugin.prototype)
		{ // if pluginPrototype is defined as a class with module.exports = <constructor name>
				pluginPrototype = plugin.prototype;
		} else {
      throw ("Plugin at " + pluginFilePath + "must be defined like a class using the prototype object. " +
         "Its functionality should be exported using the module.exports = Constructor() convention.");
    }
		
		var superInterfaceFilePath = path.join(edisonConfig.libRoot, edisonConfig.pluginInterfaceDir, edisonConfig.superInterfaceName + ".json");
		var superInterfaceSpec = JSON.parse(fs.readFileSync(superInterfaceFilePath));

		// check if interface contains properties and functions that all interfaces are required to have
		// need to do this here since pluginPrototype.component and pluginPrototype.type properties are needed below
		validateProperties(pluginPrototype, superInterfaceSpec, pluginFilePath);
		validateFunctions(pluginPrototype, superInterfaceSpec);
		
		if (!Array.isArray(pluginPrototype.components)) {
			throw ("Property 'components' in pluginPrototype '" + pluginPrototype.name +
					"' must be an array of valid component names.");
		}
		
		if (pluginPrototype.components.indexOf(component) === -1) {
			throw ("Plugin '" + pluginPrototype.name + "' was not written for component '" + component +
					"'. Please edit config file and make this pluginPrototype load for one of the following components '" +
					pluginPrototype.components + "'.");
		}
		
		var pluginInterfaceFilePath = getPluginInterfaceFilePath(pluginPrototype.components, pluginPrototype.type, component);
		var pluginInterfaceSpec = JSON.parse(fs.readFileSync(pluginInterfaceFilePath));

		validateProperties(pluginPrototype, pluginInterfaceSpec);
		validateFunctions(pluginPrototype, pluginInterfaceSpec);
		
		
		
		if (!this.loadedPlugins[component]) {
			this.loadedPlugins[component] = {};
		}
		
		if (!this.loadedPlugins[component][pluginPrototype.type]) {
			this.loadedPlugins[component][pluginPrototype.type] = [];
		} else {
			if (this.loadedPlugins[component][pluginPrototype.type].indexOf(pluginPrototype.name) !== -1) {
				console.log("INFO: Plugin '" + pluginPrototype.name + "' for component '" + component + "' already exists. Skipping...");
				continue;
			}
			
			console.log("WARNING: Following plugins with type '" + pluginPrototype.type +
					"' already exist for component '" + component + "'");
			console.log("(" + this.loadedPlugins[component][pluginPrototype.type] + ")");
			console.log("The pluginPrototype that was just loaded is " + pluginPrototype.name +
					", and it is the new active pluginPrototype for component '" + component +
					"' and type '" + pluginPrototype.type +"'");
		}
		this.loadedPlugins[component][pluginPrototype.type].push(pluginPrototype.name);
		
		callback(plugin);
	}
};

function InterfaceValidator() {
	
}

// needed to include like a class
module.exports = InterfaceValidator;
