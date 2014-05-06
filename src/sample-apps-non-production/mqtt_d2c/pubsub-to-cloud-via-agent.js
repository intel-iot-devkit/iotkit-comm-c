var edisonLib = require("../../edison-lib");

function getRandomInt(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

var validator = new edisonLib.ServiceSpecValidator();
validator.readServiceSpecFromFile("../serviceSpecs/IOTKitAgent.json");
var brokerSpec = validator.getValidatedSpec();

var count;

edisonLib.createClientForGivenService(brokerSpec, function (client) {
  console.log("Publishing");

  setInterval(function () {
    "use strict";
    count = getRandomInt(1, 10);
    setImmediate(console.log("count " + count));
    client.comm.send('{"s":"akshay", "v":' + count + '}', {topic: brokerSpec.name});
    count = getRandomInt(20, 30);
    client.comm.send('{"s":"akshay1", "v":' + count + '}', {topic: brokerSpec.name});
    setImmediate(console.log("count " + count));
  }, 1000);

});