var edison = require('../edison-lib');

// create the MDNS record for advertising
var path = require('path');

var edisonMdns = edison.getPlugin("discovery", "local");
var edisonMqtt = edison.getPlugin("communication", "pubsub");

edisonMdns.advertiseServices(path.join(edison.config.libRoot, edison.config.serviceDir));

var client = edisonMqtt.createClient('localhost', 1883);

var i = 0;
function publishData(){
	i = i + 1;
	client.publish('/Intel/temperature', '{\"cpuTemp\":' + i + '}');
	
	console.log('published message:'+i);
}

setInterval(publishData, 4000);

//client.end();