var zeromq = require('zmq');

ZeroMQ.prototype.socket = null;
ZeroMQ.prototype.component = "communication";
ZeroMQ.prototype.name = "zmqpubsub";

function ZeroMQ() {
    "use strict";
}

ZeroMQ.prototype.createService = function (serviceDescription) {
  "use strict";
  this.socket = zeromq.socket('pub');
  if (serviceDescription.address) {
    this.socket.bindSync('tcp://' + serviceDescription.address + ':' + serviceDescription.port);
  } else {
    this.socket.bindSync('tcp://*:' + serviceDescription.port);
  }
};

ZeroMQ.prototype.createClient = function (serviceDescription) {
  "use strict";
  this.socket = zeromq.socket('sub');
  this.socket.connect('tcp://' + serviceDescription.address + ':' + serviceDescription.port);
};

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