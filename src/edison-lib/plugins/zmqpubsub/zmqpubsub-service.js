var zeromq = require('zmq');

EdisonZMQPubSubService.prototype.interface = "edison-service-interface";
EdisonZMQPubSubService.prototype.socket = null;
EdisonZMQPubSubService.prototype.receivedMsgHandler = null;

function EdisonZMQPubSubService(serviceSpec) {
    "use strict";
  this.socket = zeromq.socket('pub');
  if (serviceSpec.address) {
    this.socket.bindSync('tcp://' + serviceSpec.address + ':' + serviceSpec.port);
  } else {
    this.socket.bindSync('tcp://*:' + serviceSpec.port);
  }
}

EdisonZMQPubSubService.prototype.sendTo = function (client, msg, context) {

};

EdisonZMQPubSubService.prototype.publish = function (msg, context) {
  "use strict";
  this.socket.send(msg);
};

EdisonZMQPubSubService.prototype.manageClient = function (client, context) {

};

EdisonZMQPubSubService.prototype.setReceivedMessageHandler = function (handler) {

};

EdisonZMQPubSubService.prototype.done = function () {
    this.socket.close();
};

module.exports = EdisonZMQPubSubService;