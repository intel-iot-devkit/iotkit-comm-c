var fs = require('fs');
var path = require('path');

exports.config = null;

exports.init = function (dir, configFileName) {
  "use strict";
  exports.config = JSON.parse(fs.readFileSync(path.join(dir, configFileName)));
  exports.config.pluginDir = path.join(dir, exports.config.pluginDir);
  exports.config.pluginInterfaceDir = path.join(dir, exports.config.pluginInterfaceDir);
};