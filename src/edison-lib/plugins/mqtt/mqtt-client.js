var mqtt = require('mqtt');

EdisonMQTTClient.prototype.interface = "edison-client-interface";

EdisonMQTTClient.prototype.client = {};
EdisonMQTTClient.prototype.receivedMsgHandler = null;

function EdisonMQTTClient(serviceDescription) {
  "use strict";

  if (serviceDescription.comm_params && serviceDescription.comm_params['ssl']) {
    this.client = mqtt.createSecureClient(serviceDescription.port, serviceDescription.address,
      serviceDescription.comm_params);
  } else {
    this.client = mqtt.createClient(serviceDescription.port, serviceDescription.address);
  }

  var self = this;
  this.client.on('message', function (topic, message) {
    if (self.receivedMsgHandler) {
      self.receivedMsgHandler(message, {event: 'message', topic: topic});
    }
  });
}

EdisonMQTTClient.prototype.send = function (msg, context) {
    this.client.publish(context.topic, msg);
};

EdisonMQTTClient.prototype.subscribe = function (topic) {
  this.client.subscribe(topic);
};

EdisonMQTTClient.prototype.unsubscribe = function (topic) {
  "use strict";
};

EdisonMQTTClient.prototype.setReceivedMessageHandler = function(handler) {
  "use strict";
  this.receivedMsgHandler = handler;
};

EdisonMQTTClient.prototype.done = function () {
    this.client.end();
};

module.exports = EdisonMQTTClient;