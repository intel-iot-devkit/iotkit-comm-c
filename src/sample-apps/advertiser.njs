/**
 * New node file
 */
var edisonLib = require('../edison-lib');
var path = require('path');

var discoveryService = edisonLib.getPlugin("discovery", "local");

discoveryService.advertiseServices(path.join(edisonLib.config.libRoot, edisonLib.config.serviceDir));