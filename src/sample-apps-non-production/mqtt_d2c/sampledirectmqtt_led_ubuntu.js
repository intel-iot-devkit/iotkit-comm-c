//'use strict';

var edison = require('../.');
var path = require('path');
var fs = require('fs');
var PIA = require('.');

var EdisonMqtt = edison.getPlugin("communication", "pubsub");

var args = {
    keyPath: __dirname + '/../certs/client.key',
    certPath: __dirname + '/../certs/client.crt',
    keepalive: 59000
};


var io = new PIA.IO({
    emu  : false,
    log  : 2
});

var led = new PIA.Led({
    io: io,
    pin: "DIGITAL0"
});


var client;

client = new EdisonMqtt("broker.enableiot.com", 8884, 'ssl', args);
client.subscribe('/server/metric/pradeeptmp/98-4f-ee-00-6c-02', function (topic, message) {
    "use strict";
    if(message != 'undefined' && message.s != 'undefined' && message.v != 'undefined'){
        console.log('received : ' + message);

        if(message.s === 'led'){
            switch (message.v){
                case 0:
                    led.off();
                case 1:
                    led.on();
            }
        }
    } else {
        console.log('Irrelevant message received : ' + message);
    }
});

	console.log('waiting for messages !!');
