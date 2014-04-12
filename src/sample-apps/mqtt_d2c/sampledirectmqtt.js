//'use strict';

var edison = require('/home/root/edison-api/src/edison-lib');
var path = require('path');
var fs = require('fs');

var EdisonMqtt = edison.getPlugin("communication", "pubsub");

var args = {
    keyPath: './certs/client.key',
    certPath: './certs/client.crt',
    keepalive: 59000
};

var client;

client = new EdisonMqtt("broker.enableiot.com", 8884, 'ssl', args);
client.subscribe('/server/metric/pradeeptmp/98-4f-ee-00-6c-02', function (topic, message) {
    "use strict";
    console.log(message);
});

	console.log('waiting for messages !!');
