var zeromq = require('zmq');

var socket = null;

function ZeroMQ(ip, port, type) {
    "use strict";
    if (type == 'rep') {
        socket = zeromq.socket('rep');
        console.log('rep ' + ip + ':' + port);
        socket.bindSync('tcp://*:' + port);
    } else {
        socket = zeromq.socket('req');
        console.log('req ' + ip + ':' + port);
        socket.connect('tcp://' + ip + ':' + port);
    }
}

ZeroMQ.prototype.components = ["communication"];
ZeroMQ.prototype.name = "zmqreqrep";

ZeroMQ.prototype.request = function (message) {
    socket.send(message);
};

ZeroMQ.prototype.response = function (callback) {
    socket.on('message', function (message) {
        console.log('msg ' + message);
        callback(message);
    });
};

ZeroMQ.prototype.close = function () {
    socket.close();
};

module.exports = ZeroMQ;