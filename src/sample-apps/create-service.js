var edisonLib = require('../edison-lib');

edisonLib.createService("./serviceSpecs/temperatureServiceZMQ.json", function (service) {
  "use strict";

  service.comm.setReceivedMessageHandler(function(message) {
    "use strict";
    console.log(message);
    service.comm.send(message); // echo server
  });

  setInterval(function () {
    "use strict";
    service.comm.send("mytopic", "my message");
  }, 1000);

});