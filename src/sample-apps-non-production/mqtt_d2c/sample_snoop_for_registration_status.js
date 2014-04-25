//'use strict';

var edison = require('../.');
var path = require('path');
var fs = require('fs');

var EdisonMqtt = edison.getPlugin("communication", "pubsub");

var client;

var args = {
    keyPath: __dirname + '/../certs/client.key',
    certPath: __dirname + '/../certs/client.crt',
    keepalive: 59000
};

client = new EdisonMqtt("broker.enableiot.com", 8884, 'ssl', args);
client.subscribe('/server/registration_status', function (topic, message) {
    "use strict";
    console.log(message);
});

	console.log('waiting for messages !!');
