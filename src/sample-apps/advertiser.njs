/**
 * New node file
 */
var edisonLib = require('../edison-lib');
var path = require('path');

var DiscoveryService = edisonLib.getPlugin("discovery", "local");

var discovery = new DiscoveryService();
discovery.advertiseServices(path.join(edisonLib.config.libRoot, edisonLib.config.serviceDir));