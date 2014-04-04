var edison = require('../edison-lib');

// create the MDNS record for advertising
var path = require('path');

var LocalDiscoveryService = edison.getPlugin("discovery", "local");
var PubSubClient = edison.getPlugin("communication", "pubsub");

var mdns = new LocalDiscoveryService();
mdns.advertiseServices(path.join(edison.config.libRoot, edison.config.serviceDir));

var client = new PubSubClient('localhost', 1883, 'pub');

var i = 0;
function publishData(){
    i = i + 1;
    client.publish('/Intel/temperature ', '{\"cpuTemp\":' + i + '}');

    console.log('published message:'+i);
}

setInterval(publishData, 4000);

//client.end();