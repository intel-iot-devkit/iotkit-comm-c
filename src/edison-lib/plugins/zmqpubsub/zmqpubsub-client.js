var zeromq = require('zmq');

EdisonZMQPubSubClient.prototype.interface = "edison-client-interface";
EdisonZMQPubSubClient.prototype.socket = null;
EdisonZMQPubSubClient.prototype.receivedMsgHandler = null;

function EdisonZMQPubSubClient(serviceSpec) {
  "use strict";
  this.socket = zeromq.socket('sub');
  this.socket.connect('tcp://' + serviceSpec.address + ':' + serviceSpec.port);

  var self = this;
  this.socket.on('message', function (message) {
    if (self.receivedMsgHandler) {
      self.receivedMsgHandler(message, {event: 'message'});
    }
  });
}

EdisonZMQPubSubClient.prototype.send = function (msg, context) {
  "use strict";

};

EdisonZMQPubSubClient.prototype.subscribe = function (topic) {
    this.socket.subscribe(topic);
};

EdisonZMQPubSubClient.prototype.setReceivedMessageHandler = function (handler) {
  "use strict";
  this.receivedMsgHandler = handler;
};

EdisonZMQPubSubClient.prototype.unsubscribe = function (topic) {
  "use strict";
};

EdisonZMQPubSubClient.prototype.done = function () {
    this.socket.close();
};

module.exports = EdisonZMQPubSubClient;