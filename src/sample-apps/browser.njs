var edison = require('../edison-lib');
var path = require('path');
var fs = require('fs');

var serviceSpec = JSON.parse(fs.readFileSync(path.join(edison.config.libRoot, edison.config.serviceDir, "audioService.json")));
var foundServices = [];

edison.localDirectory.discoverServices(serviceSpec.type, function(service) {
	foundServices.push(service);
	console.log("found " + foundServices.length + " services.");
});

