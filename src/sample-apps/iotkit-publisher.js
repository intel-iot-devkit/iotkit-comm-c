var edisonLib = require('edisonapi');

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("./serviceSpecs/temperatureServiceIoTKit.json");

var i = 0;
var msg = "";
edisonLib.createService(validator.getValidatedSpec(), function (service) {
    "use strict";

    // Register a Sensor by specifying its name and its type
    service.comm.registerSensor("garage","temperature.v1.0");

    setInterval(function () {
        "use strict";
        i ++;
        msg = {"n": "garage","v": i};
        service.comm.publish("data", JSON.stringify(msg) );
    }, 3000);

});


