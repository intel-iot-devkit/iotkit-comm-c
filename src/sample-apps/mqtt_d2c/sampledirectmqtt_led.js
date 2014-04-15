//'use strict';

var edison = require('/home/root/edison-api/src/edison-lib');
var path = require('path');
var fs = require('fs');
var PIA = require('../');

var EdisonMqtt = edison.getPlugin("communication", "pubsub");

var args = {
    keyPath: './certs/client.key',
    certPath: './certs/client.crt',
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


//put the event ready's handler at the last
io.on('ready', function(){

client.subscribe('/server/metric/pradeeptmp/98-4f-ee-00-6c-02', function (topic, message) {
    "use strict";                           
    var msg = JSON.parse(message);
    if(msg != 'undefined' && msg.s != 'undefined' && msg.v != 'undefined'){
        console.log('received : ' + msg);
console.log(';;;;' + msg.s + ':::' + msg.v);
                                  
        if(msg.s == 'led'){       
            switch (msg.v){                                     
                case 0:                                         
                    console.log('got into 0000000000000000000000');
                    led.off();                               
                    break; 
                case 1:                                       
                    console.log('got into 1111111111111111111111');  
                    led.on();                                 
                    break;
            }                                  
        }                                      
    } else {                                   
        console.log('Irrelevant message received : ' + message);
    }
});


console.log('waiting for messages !!'); 
});

