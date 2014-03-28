var edison = require('../edison-lib');

// create the MDNS record for advertising
var path = require('path');

var edisonMdns = edison.localDirectory.edisonMdns;
var edisonMqtt = edison.localComm.edisonMqtt;

edisonMdns.advertiseServices(path.join(edison.config.libRoot, edison.config.serviceDir));

var client = edisonMqtt.createClient('10.24.113.6', 1883);
setInterval(publishData, 4000);


var i = 0;
function publishData(){
	i = i + 1;
	client.publish('/Intel/temperature', '{\"cpuTemp\":' + i + '}');
	
	console.log('published message:'+i);
}

//client.end();