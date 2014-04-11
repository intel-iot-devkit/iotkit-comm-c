// Client
var edisonLib = require('../edison-lib');

var LocalDiscoveryService = edisonLib.getPlugin("discovery", "local");
var Comm = edisonLib.getPlugin("communication", "reqrep");

var mdns = new LocalDiscoveryService();

var serviceType = {
    "name": "zmq",
    "protocol" : "tcp",
    "subtypes" : ["cpuTemp"]
};

var client;

mdns.discoverServices(serviceType, null, onDiscovery);
function onDiscovery(service, bestAddresses){
    console.log("found " + service.type.name + " service at " + bestAddresses[0] + ":" + service.port);

    client = new Comm(bestAddresses[0], service.port, 'req');

    var i = 0;
    setInterval(function() {
        i = i + 1;
        client.request('Hello ' + i);

        console.log('Send message:'+i);
    }, 4000);

    client.response(function (message) {
        console.log('msg' + message);
    });
}
