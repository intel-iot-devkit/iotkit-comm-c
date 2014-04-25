var zeromq = require('zmq');

EdisonZMQReqRepService.prototype.interface = "edison-service-interface";
EdisonZMQReqRepService.prototype.socket = null;
EdisonZMQReqRepService.prototype.receivedMsgHandler = null;

function EdisonZMQReqRepService(serviceDescription) {
  "use strict";
  this.socket = zeromq.socket('rep');
  if (serviceDescription.address) {
    this.socket.bindSync('tcp://' + serviceDescription.address + ':' + serviceDescription.port);
  } else {
    this.socket.bindSync('tcp://*:' + serviceDescription.port);
  }

  var self = this;
  this.socket.on('message', function (message) {
    if (self.receivedMsgHandler) {
      self.receivedMsgHandler(self.socket, message, {event: 'message'});
    }
  });
}

EdisonZMQReqRepService.prototype.sendTo = function (client, msg, context) {
  "use strict";
  this.socket.send(msg);
};

EdisonZMQReqRepService.prototype.publish = function (msg, context) {
  "use strict";
};

EdisonZMQReqRepService.prototype.manageClient = function (client, context) {
};

EdisonZMQReqRepService.prototype.setReceivedMessageHandler = function (handler) {
  "use strict";
  this.receivedMsgHandler = handler;
};

EdisonZMQReqRepService.prototype.done = function () {
  this.socket.close();
};

module.exports = EdisonZMQReqRepService;