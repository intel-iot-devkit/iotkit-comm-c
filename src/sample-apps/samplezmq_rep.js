// Server
var edison = require('../edison-lib');

// create the MDNS record for advertising
var path = require('path');

var LocalDiscoveryService = edison.getPlugin("discovery", "local");
var edisonZmq = edison.getPlugin("communication", "reqrep");

var mdns = new LocalDiscoveryService();
mdns.advertiseServices(path.join(edison.config.libRoot, edison.config.serviceDir));

var server = new edisonZmq('localhost', 1883, 'rep');

server.response(function (message) {
    console.log('msg' + message);
    server.request('ok');
});
