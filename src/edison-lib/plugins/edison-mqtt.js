var mqtt = require('mqtt');

EdisonMQTT.prototype.client = null;
EdisonMQTT.prototype.component = "communication";
EdisonMQTT.prototype.name = "mqtt";

function EdisonMQTT() {
  "use strict";
}

EdisonMQTT.prototype.createService = function (serviceDescription) {
  "use strict";

};

EdisonMQTT.prototype.createClient = function (serviceDescription) {
  "use strict";
  if (serviceDescription.comm_params && serviceDescription.comm_params['ssl']) {
    this.client = mqtt.createSecureClient(serviceDescription.port, serviceDescription.address,
      serviceDescription.comm_params);
  } else {
    this.client = mqtt.createClient(serviceDescription.port, serviceDescription.address);
  }
};

EdisonMQTT.prototype.send = function (msg) {
    this.client.publish(msg.topic, msg.text);
};

EdisonMQTT.prototype.subscribe = function (topic) {
    this.client.subscribe(topic);
};

EdisonMQTT.prototype.unsubscribe = function (topic) {
  "use strict";
  console.log("unsubscribe mqtt.");
};

EdisonMQTT.prototype.setReceivedMessageHandler = function(callback) {
  "use strict";
  this.client.on('message', function (topic, message) {
    callback({topic: topic, text: message});
  });
};

EdisonMQTT.prototype.done = function () {
	this.client.close();
};

module.exports = EdisonMQTT;