var ServiceDescriptionValidator = require("../core/ServiceDescriptionValidator.js");
var ServiceRecord = require("../core/ServiceRecord.js");

var validator = new ServiceDescriptionValidator();
validator.readServiceDescriptionFromFile("../../sample-apps/serviceSpecs/temperatureServiceZMQ.json");

var record = new ServiceRecord(validator.getValidatedDescription());
console.log(record.rawRecord);

var newrecord = new ServiceRecord();
newrecord.initFromRawServiceRecord(record.rawRecord);

console.log(newrecord.serviceDescription);