var mqtt = require('mqtt');

EdisonIoTKitClient.prototype.interface = "edison-client-interface";

EdisonIoTKitClient.prototype.client = {};
EdisonIoTKitClient.prototype.receivedMsgHandler = null;

function EdisonIoTKitClient(serviceSpec) {
    "use strict";

    this.client = mqtt.createClient(serviceSpec.port, serviceSpec.address);

    var self = this;
    this.client.on('message', function (topic, message) {
        if (self.receivedMsgHandler) {
            self.receivedMsgHandler(message, {event: 'message', topic: topic});
        }
    });
}

EdisonIoTKitClient.prototype.send = function (msg, context) {
    this.client.publish(context.topic, msg);
};

EdisonIoTKitClient.prototype.subscribe = function (topic) {
    this.client.subscribe(topic, this.receivedMsgHandler);
};

EdisonIoTKitClient.prototype.unsubscribe = function (topic) {
    this.client.unsubscribe(topic);
};

EdisonIoTKitClient.prototype.setReceivedMessageHandler = function(handler) {
    "use strict";
    this.receivedMsgHandler = handler;
};

EdisonIoTKitClient.prototype.done = function () {
    this.client.end();
};

module.exports = EdisonIoTKitClient;