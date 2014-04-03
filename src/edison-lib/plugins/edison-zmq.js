var zeromq = require('zmq');

var pubsocket, subsocket;

function ZeroMQ(ip, port, type) {
  "use strict";
}

ZeroMQ.prototype.components = ["communication"];
ZeroMQ.prototype.name = "edisonZmq";
ZeroMQ.prototype.type = "pubsub";

ZeroMQ.prototype.publishTo = function (ip, port) {
  pubsocket = zeromq.socket('pub');
  pubsocket.bindSync('tcp://*:' + port);
}

ZeroMQ.prototype.subscribeFrom = function (ip, port) {
  "use strict";
  subsocket = zeromq.socket('sub');
  subsocket.connect('tcp://' + ip + ':' + port);
}

ZeroMQ.prototype.publish = function (topic, message) {
    pubsocket.send(topic + ' ' + message);
};

ZeroMQ.prototype.subscribe = function (topic, callback) {
    subsocket.subscribe(topic);

    subsocket.on('message', function (message) {
        callback(topic, message);
    });
};

ZeroMQ.prototype.close = function () {
    subsocket.close();
};

module.exports = ZeroMQ;