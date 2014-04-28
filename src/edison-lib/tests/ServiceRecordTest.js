var ServiceDescriptionValidator = require("../core/ServiceSpecValidator.js");
var ServiceRecord = require("../core/ServiceRecord.js");

var validator = new ServiceSpecValidator();
validator.readServiceSpecFromFile("../../sample-apps/serviceSpecs/temperatureServiceZMQPUBSUB.json");

var record = new ServiceRecord(validator.getValidatedSpec());
console.log(record.rawRecord);

var newrecord = new ServiceRecord();
newrecord.initFromRawServiceRecord(record.rawRecord);

console.log(newrecord.spec);