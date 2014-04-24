var mqtt = require('mqtt');

EdisonMQTT.prototype.client = {};
EdisonMQTT.prototype.service = {};
EdisonMQTT.prototype.receivedMsgHandler = null;
EdisonMQTT.prototype.component = "communication";
EdisonMQTT.prototype.name = "mqtt";

function EdisonMQTT() {
  "use strict";
}

EdisonMQTT.prototype.createService = function (serviceDescription) {
  "use strict";

  this.service = mqtt.createServer(function(client) {
    var self = this;

    client.on('connect', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler('connect', client, packet);
      }
    });

    client.on('publish', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler('publish', client, packet);
      }
    });

    client.on('subscribe', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler('subscribe', client, packet);
      }
    });

    client.on('pingreq', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler('pingreq', client, packet);
      }
    });

    client.on('disconnect', function(packet) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler('disconnect', client, packet);
      }
    });

    client.on('close', function(err) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler('close', client, err);
      }
    });

    client.on('error', function(err) {
      if (self.receivedMsgHandler) {
        self.receivedMsgHandler('error', client, err);
      }
    });
  });

  this.service.listen(serviceDescription.port);
};

EdisonMQTT.prototype.createClient = function (serviceDescription) {
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
      self.receivedMsgHandler('message', this.client, {topic: topic, text: message});
    }
  });
};

EdisonMQTT.prototype.send = function (channel, msg, context) {
    channel.publish(msg.topic, msg.text);
};

EdisonMQTT.prototype.subscribe = function (topic) {
  if (this.client) {
    this.client.subscribe(topic);
  }
};

EdisonMQTT.prototype.unsubscribe = function (topic) {
  "use strict";
  console.log("unsubscribe mqtt.");
};

EdisonMQTT.prototype.setReceivedMessageHandler = function(handler) {
  "use strict";
  this.receivedMsgHandler = handler;
};

EdisonMQTT.prototype.done = function () {
  if (this.client) {
    this.client.end();
  }
};

EdisonMQTT.prototype.manageChannel = function (channel, actions) {
  "use strict";
  if (actions.kill) {
    channel.stream.end;
  }
};

EdisonMQTT.prototype.getDefaultChannel = function () {
  "use strict";

  if (this.client) {
    return this.client;
  }

  return null;
};

module.exports = EdisonMQTT;