var fs = require('fs');
var path = require('path');

var CORE_PLUGINS_DIR_NAME = "plugins";
var CORE_PLUGIN_INTERFACE_DIR_NAME = "plugin-interfaces";
var PLUGIN_SUPER_INTERFACE_FILE_NAME = "interface-for-interfaces.json"

exports.config = null;

exports.init = function (dir, configFileName) {
  "use strict";
  exports.config = JSON.parse(fs.readFileSync(path.join(dir, configFileName)));
  exports.config.pluginDirPaths.push(path.join(dir, CORE_PLUGINS_DIR_NAME));
  var pluginInterfaceDirPath = path.join(dir, CORE_PLUGIN_INTERFACE_DIR_NAME);
  exports.config.pluginInterfaceDirPaths.push(pluginInterfaceDirPath);
  exports.config.superInterfaceFilePath = path.join(pluginInterfaceDirPath, PLUGIN_SUPER_INTERFACE_FILE_NAME);
};