var edisonConfig = require("./../config.js");
var path = require('path');
var fs = require('fs');

// if pluginFilePath is not supplied, it is assumed that property plugin.name exists
function validateProperties(plugin, interfaceSpec, pluginFilePath) {
	if(!interfaceSpec.properties) {
		return;
	}
	
	// check if all required properties exist
	for (var i in interfaceSpec.properties) {
		if (!plugin[interfaceSpec.properties[i]]) {
			if(pluginFilePath) {
				throw new Error("Plugin at '" + pluginFilePath + "' does not define required property '" + interfaceSpec.properties[i] + "'.");
			} else {
				throw new Error("Plugin '" + plugin.name + "' does not define required property '" + interfaceSpec.properties[i] + "'.");
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
			throw new Error("Plugin '" + plugin.name + "' does not define required function '" + interfaceSpec.functions[j] + "'.");
		}
	}
	
	// check that type is indeed a function
	for (var i in interfaceSpec.functions) {
		if ((typeof plugin[interfaceSpec.functions[i]]) !== "function") {
			throw new Error("Plugin '" + plugin.name + "' does not define '" + interfaceSpec.functions[i] + "' as a function.");
		}
	}
}

function getPluginFilePath(pluginDescriptor) {
	return path.join(edisonConfig.libRoot, edisonConfig.pluginDir, pluginDescriptor.fileName);
}

function getPluginInterfaceFilePath(pluginPrototype) {
  var currentPath = path.join(edisonConfig.libRoot,
    edisonConfig.pluginInterfaceDir,
    pluginPrototype.component + ".json");

  if (!fs.existsSync(currentPath))
  { // no interface file exists for this plugin
    throw ("No interface specification file found for plugin '" + plugin.name +
      "'. Plugin expects an interface file named '" + pluginPrototype.component + ".json'.");
  }

  return currentPath;
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
		// need to do this here since pluginPrototype.component is needed below
		validateProperties(pluginPrototype, superInterfaceSpec, pluginFilePath);
		validateFunctions(pluginPrototype, superInterfaceSpec);

    if (pluginPrototype.component !== component) {
      throw ("Plugin '" + pluginPrototype.name + "' is not correctly configured in 'config.js'. " +
        "The plugin configuration line-item must be moved under component '" + pluginPrototype.component + "'.");
    }

		var pluginInterfaceFilePath = getPluginInterfaceFilePath(pluginPrototype);
		var pluginInterfaceSpec = JSON.parse(fs.readFileSync(pluginInterfaceFilePath));

    validateProperties(pluginPrototype, pluginInterfaceSpec);
    validateFunctions(pluginPrototype, pluginInterfaceSpec);

    if (!this.loadedPlugins[component]) {
      this.loadedPlugins[component] = [];
    }

    if (this.loadedPlugins[component].indexOf(pluginPrototype.name) !== -1) {
      console.log("INFO: Plugin '" + pluginPrototype.name + "' for component '" + component + "' already exists. Skipping...");
      continue;
    }

    this.loadedPlugins[component].push(pluginPrototype.name);

    callback(component, plugin);
	}
};

function InterfaceValidator() {
	
}

// needed to include like a class
module.exports = InterfaceValidator;
