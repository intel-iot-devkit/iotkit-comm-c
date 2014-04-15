var edison = require('../../edison-lib');

// create the MDNS record for advertising
var path = require('path');

var EdisonMqtt = edison.getPlugin("communication", "pubsub");

var args = {
    keyPath: __dirname + '/../certs/client.key',
    certPath: __dirname + '/../certs/client.crt',
    keepalive: 59000
};

var client = new EdisonMqtt('broker.enableiot.com', 8884, 'ssl', args);

//client.publish('/server/metric/pradeeptmp/98-4f-ee-00-6c-02', '{\"s\":\"led\",\"v\":0}');

var i = 0;
//function publishData(){
//	i = i + 1;

// toggle between on and off
for(i=0;i<25;i++){
        if(i % 2 == 0){
            client.publish('/foo', '{\"s\":\"led\",\"v\":0}');
        }else {
            client.publish('/foo', '{\"s\":\"led\",\"v\":1}');
        }
	//client.publish('data', '{\"s\":\"mysensor2\",\"v\":22.2}');
	
	console.log('published message:'+i);
}

//setInterval(publishData, 1000);
//?process.exit();
//client.close();
