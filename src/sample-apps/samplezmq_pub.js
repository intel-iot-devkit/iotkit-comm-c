var edison = require('../edison-lib');

// create the MDNS record for advertising
var path = require('path');

var edisonMdns = edison.getPlugin("discovery", "local");
var edisonZmq = edison.getPlugin("communication", "pubsub");
var client;

edisonMdns.advertiseServices(path.join(edison.config.libRoot, edison.config.serviceDir));

edisonZmq.createPubClient('localhost', 1883, function(clientD) {
    client = clientD;
});

var i = 0;
function publishData(){
    i = i + 1;
    client.send('/Intel/temperature ' + '{\"cpuTemp\":' + i + '}');

    console.log('published message:'+i);
}

setInterval(publishData, 4000);

//client.end();