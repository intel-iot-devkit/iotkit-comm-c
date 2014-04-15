var zeromq = require('zmq');

var socket;

function ZeroMQ(ip, port, type) {
    "use strict";
    if (type == 'pub') {
        socket = zeromq.socket('pub');
        console.log('pub ' + ip + ':' + port);
        socket.bindSync('tcp://*:' + port);
    } else {
        socket = zeromq.socket('sub');
        console.log('sub ' + ip + ':' + port);
        socket.connect('tcp://' + ip + ':' + port);
    }
}

ZeroMQ.prototype.components = ["communication"];
ZeroMQ.prototype.name = "edisonZmq";
ZeroMQ.prototype.type = "pubsub";

ZeroMQ.prototype.publish = function (topic, message) {
    socket.send(topic + ' ' + message);
};

ZeroMQ.prototype.subscribe = function (topic, callback) {
    socket.subscribe(topic);

    socket.on('message', function (message) {
        callback(topic, message);
    });
};

ZeroMQ.prototype.close = function () {
    socket.close();
};

module.exports = ZeroMQ;