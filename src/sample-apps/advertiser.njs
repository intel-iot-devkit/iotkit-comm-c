/**
 * New node file
 */
var edison = require('../edison-lib');
var path = require('path');

var edisonMdns = edison.localDirectory.edisonMdns;

edisonMdns.advertiseServices(path.join(edison.config.libRoot, edison.config.serviceDir));