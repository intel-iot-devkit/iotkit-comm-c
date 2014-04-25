var mqtt = require('mqtt');

EdisonMQTTService.prototype.interface = "edison-service-interface";

EdisonMQTTService.prototype.service = {};
EdisonMQTTService.prototype.receivedMsgHandler = null;

function EdisonMQTTService(serviceDescription) {
  "use strict";

  var self = this;
  this.service = mqtt.createServer(function(client) {
    client.on('connect', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, packet, {event: 'connect'});
      }
    });

    client.on('publish', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, packet, {event: 'publish'});
      }
    });

    client.on('subscribe', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, packet, {event: 'subscribe'});
      }
    });

    client.on('pingreq', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, packet, {event: 'pingreq'});
      }
    });

    client.on('disconnect', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, packet, {event: 'disconnect'});
      }
    });

    client.on('close', function(err) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, err, {event: 'close'});
      }
    });

    client.on('error', function(err) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler(client, err, {event: 'error'});
      }
    });
  });

  this.service.listen(serviceDescription.port);
}

EdisonMQTTService.prototype.sendTo = function (client, msg, context) {

  if (!context) {
    client.publish(msg);
    return;
  }

  switch (context.ack) {
    case 'connack':
      client.connack({returnCode: 0});
      break;
    case 'suback':
      var granted = [];
      for (var i = 0; i < msg.subscriptions.length; i++) {
        granted.push(msg.subscriptions[i].qos);
      }
      client.suback({granted: granted, messageId: msg.messageId});
      break;
    case 'pingresp':
      client.pingresp();
      break;
    default:
      client.publish(msg);
  }
};

EdisonMQTTService.prototype.publish = function (msg, context) {
  "use strict";

};

EdisonMQTTService.prototype.setReceivedMessageHandler = function(handler) {
  "use strict";
  this.receivedMsgHandler = handler;
};

EdisonMQTTService.prototype.done = function () {
};

EdisonMQTTService.prototype.manageClient = function (client, context) {
  "use strict";
  switch (context.action) {
    case 'endstream':
      client.stream.end();
  }
};

module.exports = EdisonMQTTService;