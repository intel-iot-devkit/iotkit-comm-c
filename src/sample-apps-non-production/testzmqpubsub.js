var zmq = require('zmq')

var pub = zmq.socket('pub')
  , sub = zmq.socket('sub');

var n = 0;

sub.subscribe('');
sub.on('message', function(msg){
  console.log(msg.toString());
  switch (n++) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      sub.close();
      pub.close();
      break;
  }
});

var addr = "tcp://127.0.0.1:1883";

sub.bind(addr, function(){
  pub.connect(addr);

  // The connect is asynchronous, and messages published to a non-
  // connected socket are silently dropped.  That means that there is
  // a race between connecting and sending the first message which
  // causes this test to hang, especially when running on Linux. Even an
  // inproc:// socket seems to be asynchronous.  So instead of
  // sending straight away, we wait 100ms for the connection to be
  // established before we start the send.  This fixes the observed
  // hang.

  setTimeout(function() {
    pub.send('foo');
    pub.send('bar');
    pub.send('baz');
  }, 100.0);
});