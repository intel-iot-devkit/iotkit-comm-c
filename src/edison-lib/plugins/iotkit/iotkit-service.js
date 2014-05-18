var mqtt = require('mqtt');

EdisonIoTKitService.prototype.interface = "edison-iotkit-service-interface";

EdisonIoTKitService.prototype.service = {};
EdisonIoTKitService.prototype.receivedMsgHandler = null;

function EdisonIoTKitService(serviceSpec) {
    "use strict";

    if (serviceSpec.comm_params && serviceSpec.comm_params['ssl']) {
        this.client = mqtt.createSecureClient(serviceSpec.port, serviceSpec.address,
            serviceSpec.comm_params.args);
    } else {
        this.client = mqtt.createClient(serviceSpec.port, serviceSpec.address);
    }

    var self = this;
    this.client.on('message', function (topic, message) {
        if (self.receivedMsgHandler) {
            self.receivedMsgHandler(message, {event: 'message', topic: topic});
        }
    });
}

EdisonIoTKitService.prototype.registerSensor = function(sensorname, type, unit){
    this.client.publish("data", JSON.stringify({"n":sensorname, "t": type}));
}

EdisonIoTKitService.prototype.sendTo = function (msg, context) {
    this.client.publish(context.topic, msg);
};

EdisonIoTKitService.prototype.publish = function (topic, msg) {
    this.client.publish(topic, msg);
};

EdisonIoTKitService.prototype.manageClient = function (topic) {
    "use strict";
};

EdisonIoTKitService.prototype.setReceivedMessageHandler = function(handler) {
    "use strict";
    this.receivedMsgHandler = handler;
};

EdisonIoTKitService.prototype.done = function () {
    this.client.end();
};

module.exports = EdisonIoTKitService;