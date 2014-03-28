var edison = require('../edison-lib');

// create the MDNS record for advertising
var path = require('path');


console.log('let us advertize');
edison.localDirectory.advertiseServices(path.join(edison.config.libRoot, edison.config.serviceDir));
console.log('OK advertised now');

console.log('let me create publisher client');
var client = edison.localComm.createClient('localhost', 1883);
console.log('OK created');

console.log('let me subscribe to a topic');
client.subscribe('/intel/temperature');
console.log('OK subscribed');

console.log('let me publish some message');
client.publish('/intel/temperature', '{\"cpuTemp\":32.4}');
console.log('OK published');

client.end();