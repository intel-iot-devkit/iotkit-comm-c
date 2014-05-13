var path = require('path');
var expect = require('chai').expect;

describe('#mqtt-mini-broker', function () {
  it("tests if mqtt mini broker started correctly", function(done) {
    var edisonLib = require('edisonapi');

    var validator = new edisonLib.ServiceSpecValidator();
    validator.readServiceSpecFromFile(path.join(__dirname, "serviceSpecs/temperatureServiceMQTT.json"));

    edisonLib.createService(validator.getValidatedSpec(), function (service) {
      "use strict";

      var clients = {};

      service.comm.setReceivedMessageHandler(function(client, msg, context) {
        "use strict";

        switch (context.event) {
          case 'connect':
            clients[msg.clientId] = client;
            service.comm.sendTo(client, msg, {ack: 'connack'});
            break;
          case 'publish':
            for (var clientId in clients) {
              if (!clients.hasOwnProperty(clientId))
                continue;
              service.comm.sendTo(clients[clientId], msg);
            }
            break;
          case 'subscribe':
            service.comm.sendTo(client, msg, {ack: 'suback'});
            break;
          case 'close':
            for (var clientId in clients) {
              if (!clients.hasOwnProperty(clientId))
                continue;
              if (clients[clientId] == client) {
                delete clients[clientId];
              }
            }
            break;
          case 'pingreq':
            service.comm.sendTo(client, msg, {ack: 'pingresp'});
            break;
          case 'disconnect':
          case 'error':
            service.comm.manageClient(client, {action: 'endstream'});
            break;
          default:
            console.log(context.event);
        }
      });

      expect(service.comm).to.not.be.undefined;
      done();
    });
  });
});


