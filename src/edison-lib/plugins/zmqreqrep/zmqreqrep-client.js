var zeromq = require('zmq');

EdisonZMQReqRepClient.prototype.interface = "edison-client-interface";
EdisonZMQReqRepClient.prototype.socket = null;
EdisonZMQReqRepClient.prototype.receivedMsgHandler = null;

function EdisonZMQReqRepClient(serviceSpec) {
  "use strict";
  this.socket = zeromq.socket('req');
  this.socket.connect('tcp://' + serviceSpec.address + ':' + serviceSpec.port);

  var self = this;
  this.socket.on('message', function (message) {
    if (self.receivedMsgHandler) {
      self.receivedMsgHandler(message, {event: 'message'});
    }
  });
}

EdisonZMQReqRepClient.prototype.send = function (msg, context) {
  "use strict";
  this.socket.send(msg);
};

EdisonZMQReqRepClient.prototype.subscribe = function (topic) {
};

EdisonZMQReqRepClient.prototype.setReceivedMessageHandler = function (handler) {
  "use strict";
  this.receivedMsgHandler = handler;
};

EdisonZMQReqRepClient.prototype.unsubscribe = function (topic) {
  "use strict";
};

EdisonZMQReqRepClient.prototype.done = function () {
    this.socket.close();
};

module.exports = EdisonZMQReqRepClient;