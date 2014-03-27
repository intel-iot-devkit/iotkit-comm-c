var edisonConfig = require("./config.js").config;
var fs = require('fs');

// Verifies that a plugin confirms to the interface type it claims to be.
function InterfaceValidator(pluginFilePath) {
	var plugin = require(pluginFilePath);
	
	switch(plugin.type) {
	case "localdiscovery":
		this.interfaceSpec = JSON.parse(fs.readFileSync(edisonConfig.libroot + "/" + edisonConfig.interfaceDir + "/" + plugin.type + ".json"));
		this.interfaceInstance = plugin;
		break;
	default:
		throw("No such interface");
	}
}
// vars
InterfaceValidator.prototype.interfaceSpec = {}; // JSON object
InterfaceValidator.prototype.interfaceInstance = null; // Object

// methods
InterfaceValidator.prototype.validate = function()
{	
	// validate instance contains the specified functions
	for (var i in this.interfaceSpec.functions) {
		if ((typeof this.interfaceInstance[this.interfaceSpec.functions[i]]) !== "function") {
			throw("Plugin '" + this.interfaceInstance.name + "' does not define required function '" + this.interfaceSpec.functions[i] + "'.");
		}
	}
};

// needed to include like a class
module.exports = InterfaceValidator;
