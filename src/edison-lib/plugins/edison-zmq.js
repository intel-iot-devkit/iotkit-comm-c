var zeromq = require('zmq');

var pubsocket, subsocket;

function ZeroMQ(ip, port, type) {
  "use strict";

  if (type === 'pub') {
    pubsocket = zeromq.socket('pub');
    pubsocket.bindSync('tcp://*:' + port);
  } else if (type === 'sub' ) {
    subsocket = zeromq.socket('sub');
    subsocket.connect('tcp://' + ip + ':' + port);
  } else {
    throw ("Possible types are 'pub' or 'sub'");
  }
}

ZeroMQ.prototype.components = ["communication"];
ZeroMQ.prototype.name = "edisonZmq";
ZeroMQ.prototype.type = "pubsub";

ZeroMQ.prototype.publish = function (topic, message) {
    pubsocket.send(topic + ' ' + message);
};

ZeroMQ.prototype.subscribe = function (topic, callback) {
    subsocket.subscribe(topic);

    subsocket.on('message', function (message) {
        console.log(__filename, "sub received:" + topic + message);
        callback(topic, message);
    });
};

ZeroMQ.prototype.close = function () {
    subsocket.close();
};

module.exports = ZeroMQ;