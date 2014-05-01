var path = require('path');
var fs = require('fs');

var config = require('./config-manager.js').config;

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
  var foundPath = "";
  var found = config.pluginInterfaceDirPaths.some(function (pluginInterfaceDirPath) {
    var currPath = path.join(pluginInterfaceDirPath, pluginPrototype.interface + ".json");
    if (fs.statSync(currPath)) {
      foundPath = currPath;
      return true;
    }
  });

  if (!found) {
    throw new Error("Could not find plugin interface file '" + pluginPrototype.interface + ".json" +
      "' required by plugin '" + pluginName + "' in any of the configured plugin interface directories.");
  }

  return foundPath;
}

function getPluginDirectoryPath(pluginName) {
  "use strict";
  var foundPath = "";
  var found = config.pluginDirPaths.some(function (pluginDirPath) {
    var currPath = path.join(pluginDirPath, pluginName);
    if (fs.statSync(currPath)) {
      foundPath = currPath;
      return true;
    }
  });

  if (!found) {
    throw new Error("Could not find plugin '" + pluginName + "' in any of the configured plugin directories.");
  }

  return foundPath;
}

// format: { "type": ["plugin1", "plugin2"]}
exports.loadedPlugins = {};

exports.config = require('./config-manager.js').config;

// methods

//Verifies that a plugin confirms to the interface type it claims to be.
exports.loadPlugin = function(pluginName)
{
  var pluginDirectoryPath = getPluginDirectoryPath(pluginName);
  var suffixKeys = Object.keys(config.communicationPlugins.fileSuffixes);

	for (var i = 0; i < suffixKeys.length; i++) {
    var suffix = config.communicationPlugins.fileSuffixes[suffixKeys[i]];
    var pluginFileName = pluginName + '-' + suffix + '.js';
		var plugin = require(path.join(pluginDirectoryPath, pluginFileName));
		var pluginPrototype = null;

		if (typeof plugin === "function" && plugin.prototype)
		{ // if pluginPrototype is defined as a class with module.exports = <constructor name>
				pluginPrototype = plugin.prototype;
		} else {
      throw ("Plugin " + pluginName + " must be defined like a class using the prototype object. " +
         "Its functionality should be exported using the module.exports = Constructor() convention.");
    }

    // todo: the path at which the super interface is available needs to be a constant and not dependent on anything else.
		var superInterfaceSpec = JSON.parse(fs.readFileSync(config.superInterfaceFilePath));

		// check if interface contains properties and functions that all interfaces are required to have
		// need to do this here since pluginPrototype.interface is needed below
		validateProperties(pluginPrototype, pluginFileName, superInterfaceSpec);
		validateFunctions(pluginPrototype, pluginFileName, superInterfaceSpec);

		var pluginInterfaceFilePath = getPluginInterfaceFilePath(pluginPrototype, pluginName);
		var pluginInterfaceSpec = JSON.parse(fs.readFileSync(pluginInterfaceFilePath));

    validateProperties(pluginPrototype, pluginFileName, pluginInterfaceSpec);
    validateFunctions(pluginPrototype, pluginFileName, pluginInterfaceSpec);

    if (!exports.loadedPlugins[pluginName]) {
      exports.loadedPlugins[pluginName] = {};
    }

    if (exports.loadedPlugins[pluginName][suffix]) {
      console.log("INFO: Plugin file '" + pluginName + '-' + suffix + ".js' already loaded. Skipping...");
      continue;
    }

    exports.loadedPlugins[pluginName][suffix] = plugin;
	}
};

exports.getClientPlugin = function (name) {
  if (!exports.loadedPlugins[name]) {
    this.loadPlugin(name);
  }

  return exports.loadedPlugins[name][config.communicationPlugins.fileSuffixes.clientFileSuffix];
};

exports.getServicePlugin = function (name) {
  if (!exports.loadedPlugins[name]) {
    this.loadPlugin(name);
  }

  return exports.loadedPlugins[name][config.communicationPlugins.fileSuffixes.serverFileSuffix];
};

