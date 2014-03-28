/**
 * New node file
 */
var edison = require('../edison-lib');
var path = require('path');

edison.localDirectory.advertiseServices(path.join(edison.config.libRoot, edison.config.serviceDir));