var zeromq = require('zmq');

var socket;
socket = zeromq.socket('sub');
socket.connect('tcp://127.0.0.1:1883');
socket.subscribe('');
socket.on('message', function (message) {
  console.log(message.toString());
});

