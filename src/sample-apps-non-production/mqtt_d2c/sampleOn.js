var edison = require('../.');

// create the MDNS record for advertising
var path = require('path');

var EdisonMqtt = edison.getPlugin("communication", "pubsub");

var args = {
    keyPath: __dirname + '/../certs/client.key',
    certPath: __dirname + '/../certs/client.crt',
    keepalive: 59000
};

var client = new EdisonMqtt('broker.enableiot.com', 8884, 'ssl', args);

client.publish('/server/metric/pradeeptmp/98-4f-ee-00-6c-02', '{\"s\":\"led\",\"v\":1}');

/*var i = 0;
function publishData(){
	i = i + 1;
	client.publish('data', '{\"s\":\"mysensor2\",\"v\":22.2}');
	
	console.log('published message:'+i);
}*/

//setInterval(publishData, 4000);
//process.exit();
//client.close();
