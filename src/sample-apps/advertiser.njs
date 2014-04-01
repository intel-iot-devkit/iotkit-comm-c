/**
 * New node file
 */
var edison = require('../edison-lib');
var path = require('path');

var discoveryService = edison.localDirectory.basic;

discoveryService.advertiseServices(path.join(edison.config.libRoot, edison.config.serviceDir));