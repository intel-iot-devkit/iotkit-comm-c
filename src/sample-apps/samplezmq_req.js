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

mdns.discoverServices(serviceType, onDiscovery);
function onDiscovery(service){
    console.log("found " + service.type.name + " service at " + service.addresses[service.addresses.length-1] + ":" + service.port);

    client = new Comm(service.addresses[service.addresses.length-1], service.port, 'req');

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
