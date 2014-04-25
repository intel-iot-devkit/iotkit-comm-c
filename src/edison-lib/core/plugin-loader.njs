var edisonConfig = require("./../config.js");
var path = require('path');
var fs = require('fs');

// if pluginFilePath is not supplied, it is assumed that property plugin.name exists
function validateProperties(pluginPrototype, pluginFile, interfaceSpec) {
  if(!interfaceSpec.properties) {
    return;
  }

  // check if all required properties exist
  for (var i in interfaceSpec.properties) {
    if (!pluginPrototype[interfaceSpec.properties[i]]) {
      throw new Error("Plugin '" + pluginFile + "' does not define required property '" + interfaceSpec.properties[i] + "'.");
    }
  }
}

function validateFunctions(pluginPrototype, pluginFile, interfaceSpec) {
	if(!interfaceSpec.functions) {
		return;
	}
	
	// check if all required function names exist
	for (var j in interfaceSpec.functions) {
		if (!pluginPrototype[interfaceSpec.functions[j]]) {
			throw new Error("Plugin file '" + pluginFile + "' does not define required function '" + interfaceSpec.functions[j] + "'.");
		}
	}
	
	// check that type is indeed a function
	for (var i in interfaceSpec.functions) {
		if ((typeof pluginPrototype[interfaceSpec.functions[i]]) !== "function") {
			throw new Error("Plugin file '" + pluginFile + "' does not define '" + interfaceSpec.functions[i] + "' as a function.");
		}
	}
}

function getPluginInterfaceFilePath(pluginPrototype, pluginName) {
  var currentPath = path.join(edisonConfig.libRoot,
    edisonConfig.pluginInterfaceDir,
    pluginPrototype.interface + ".json");

  if (!fs.existsSync(currentPath))
  { // no interface file exists for this plugin
    throw ("Missing interface specification file for plugin '" + pluginName +
      "'. Plugin expects an interface file named '" + pluginPrototype.interface + ".json'.");
  }

  return currentPath;
}

// format: { "type": ["plugin1", "plugin2"]}
InterfaceValidator.prototype.loadedPlugins = {};

// methods

//Verifies that a plugin confirms to the interface type it claims to be.
InterfaceValidator.prototype.loadPlugin = function(pluginName, callback)
{
  var pluginDirectory = path.join(edisonConfig.libRoot, edisonConfig.pluginDir, pluginName);
  var suffixKeys = Object.keys(edisonConfig.communication.pluginFileSuffixes);

	for (var i = 0; i < suffixKeys.length; i++) {
    var suffix = edisonConfig.communication.pluginFileSuffixes[suffixKeys[i]];
    var pluginFile = pluginName + '-' + suffix + '.js';
		var plugin = require(path.join(pluginDirectory, pluginFile));
		
		if (typeof plugin === "function" && plugin.prototype)
		{ // if pluginPrototype is defined as a class with module.exports = <constructor name>
				pluginPrototype = plugin.prototype;
		} else {
      throw ("Plugin " + pluginName + " must be defined like a class using the prototype object. " +
         "Its functionality should be exported using the module.exports = Constructor() convention.");
    }

    // todo: the path at which the super interface is available needs to be a constant and not dependent on anything else.
		var superInterfaceFilePath = path.join(edisonConfig.libRoot, edisonConfig.pluginInterfaceDir, edisonConfig.superInterfaceName + ".json");
		var superInterfaceSpec = JSON.parse(fs.readFileSync(superInterfaceFilePath));

		// check if interface contains properties and functions that all interfaces are required to have
		// need to do this here since pluginPrototype.interface is needed below
		validateProperties(pluginPrototype, pluginFile, superInterfaceSpec);
		validateFunctions(pluginPrototype, pluginFile, superInterfaceSpec);

		var pluginInterfaceFilePath = getPluginInterfaceFilePath(pluginPrototype, pluginName);
		var pluginInterfaceSpec = JSON.parse(fs.readFileSync(pluginInterfaceFilePath));

    validateProperties(pluginPrototype, pluginFile, pluginInterfaceSpec);
    validateFunctions(pluginPrototype, pluginFile, pluginInterfaceSpec);

    if (!this.loadedPlugins[pluginName]) {
      this.loadedPlugins[pluginName] = {};
    }

    if (this.loadedPlugins[pluginName][suffix]) {
      console.log("INFO: Plugin file '" + pluginName + '-' + suffix + ".js' already loaded. Skipping...");
      continue;
    }

    this.loadedPlugins[pluginName][suffix] = plugin;
	}

};

function InterfaceValidator() {
	
}

// needed to include like a class
module.exports = InterfaceValidator;
