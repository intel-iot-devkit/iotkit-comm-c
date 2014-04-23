var zeromq = require('zmq');

EdisonZMQPubSub.prototype.socket = null;
EdisonZMQPubSub.prototype.component = "communication";
EdisonZMQPubSub.prototype.name = "zmqpubsub";

function EdisonZMQPubSub() {
    "use strict";
}

EdisonZMQPubSub.prototype.createService = function (serviceDescription) {
  "use strict";
  this.socket = zeromq.socket('pub');
  if (serviceDescription.address) {
    this.socket.bindSync('tcp://' + serviceDescription.address + ':' + serviceDescription.port);
  } else {
    this.socket.bindSync('tcp://*:' + serviceDescription.port);
  }
};

EdisonZMQPubSub.prototype.createClient = function (serviceDescription) {
  "use strict";
  this.socket = zeromq.socket('sub');
  this.socket.connect('tcp://' + serviceDescription.address + ':' + serviceDescription.port);
};

EdisonZMQPubSub.prototype.send = function (msg) {
    this.socket.send(msg.topic + ' ' + msg.text);
};

EdisonZMQPubSub.prototype.subscribe = function (topic) {
    this.socket.subscribe(topic);
};

EdisonZMQPubSub.prototype.setReceivedMessageHandler = function (handler) {
  "use strict";

  this.socket.on('message', function (message) {
    if (handler) {
      handler(message);
    } else {
      console.log("WARNING: No receive message handler set. Dropping message.");
    }
  });
};

EdisonZMQPubSub.prototype.unsubscribe = function () {
  "use strict";
};

EdisonZMQPubSub.prototype.done = function () {
    this.socket.close();
};

module.exports = EdisonZMQPubSub;