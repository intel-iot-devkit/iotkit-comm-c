var path = require("path");

var edisonLib = require("edisonapi");

var msgTemplate = {
  "msg_type": "metrics_msg",
  "sender_id": "b8-e8-56-37-7a-22",
  "account_id": "pradeeptmp",
  "timestamp": 1399318044904,
  "data_source": [
  {
    "name": "self",
    "metrics": [
      {
        "name": "tap",
        "sample": [
          {
            "value": "2.9",
            "timestamp": 1399318044904
          }
        ]
      }
    ]
  }
]
}

function getRandomInt(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("../serviceSpecs/IOTKitCloudBroker.json");

validator.spec.comm_params.args.keyPath = path.resolve("../serviceSpecs/", validator.spec.comm_params.args.keyPath);
validator.spec.comm_params.args.certPath = path.resolve("../serviceSpecs/", validator.spec.comm_params.args.certPath);

var brokerSpec = validator.getValidatedSpec();

var count;

edisonLib.createClientForGivenService(brokerSpec, function (client) {
  client.comm.subscribe(brokerSpec.name);

  client.comm.setReceivedMessageHandler(function(message, context) {
    "use strict";

    var m = JSON.parse(message);
    if (m.data_source[0].metrics[0].name === 'akshay'
      || m.data_source[0].metrics[0].name === 'akshay1') {
      console.log(m.data_source[0].metrics[0]);
    }

  });

  setInterval(function () {
    "use strict";
    var d = new Date();
    count = getRandomInt(1, 10);
    msgTemplate.timestamp = d.getTime();
    msgTemplate.data_source[0].metrics[0].name = 'akshay';
    msgTemplate.data_source[0].metrics[0].sample[0].value = count;
    msgTemplate.data_source[0].metrics[0].sample[0].timestamp = msgTemplate.timestamp;
    setImmediate(console.log("count " + count));
    client.comm.send(JSON.stringify(msgTemplate), {topic: brokerSpec.name});

    d = new Date();
    count = getRandomInt(20, 30);
    msgTemplate.timestamp = d.getTime();
    msgTemplate.data_source[0].metrics[0].name = 'akshay1';
    msgTemplate.data_source[0].metrics[0].sample[0].value = count;
    msgTemplate.data_source[0].metrics[0].sample[0].timestamp = msgTemplate.timestamp;
    setImmediate(console.log("count " + count));
    client.comm.send(JSON.stringify(msgTemplate), {topic: brokerSpec.name});
  }, 1000);

});