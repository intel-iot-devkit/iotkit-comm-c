var zeromq = require('zmq');

var socket;
socket = zeromq.socket('pub');
socket.bindSync('tcp://127.0.0.1:1883');
socket.on('message', function (message) {
  console.log(message);
});

setInterval(function () {
  "use strict";
  socket.send("mytopic my message");
}, 1000);
