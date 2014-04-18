var Service = require('../edison-lib').Service;

var myservice = new Service("./serviceSpecs/temperatureServiceZMQ.json");
myservice.comm.setReceivedMessageHandler(function(message) {
  "use strict";
  console.log(message);
});
myservice.advertise();