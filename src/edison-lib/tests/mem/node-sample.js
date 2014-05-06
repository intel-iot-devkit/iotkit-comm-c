var util = require('util');

function dohello () {
  console.log(util.inspect(process.memoryUsage()));
  setImmediate(dohello);
}

dohello();
