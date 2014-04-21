var zeromq = require('zmq');

ZeroMQ.prototype.socket = null;
ZeroMQ.prototype.component = "communication";
ZeroMQ.prototype.name = "zmqpubsub";

function ZeroMQ(port, type, ip) {
    "use strict";
    if (type == 'server') {
        this.socket = zeromq.socket('pub');
        this.socket.bindSync('tcp://*:' + port);
    } else {
        this.socket = zeromq.socket('sub');
        this.socket.connect('tcp://' + ip + ':' + port);
    }
}

ZeroMQ.prototype.send = function (msg) {
    this.socket.send(msg.topic + ' ' + msg.text);
};

ZeroMQ.prototype.subscribe = function (topic) {
    this.socket.subscribe(topic);
};

ZeroMQ.prototype.setReceivedMessageHandler = function (handler) {
  "use strict";

  this.socket.on('message', function (message) {
    handler(message);
  });
};

ZeroMQ.prototype.unsubscribe = function () {
  "use strict";
};

ZeroMQ.prototype.done = function () {
    this.socket.close();
};

module.exports = ZeroMQ;