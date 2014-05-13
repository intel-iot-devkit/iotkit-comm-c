var expect = require('chai').expect;
var path = require('path');

describe('ServiceSpec', function () {
  describe('#goodspec', function () {
    it("should validate this spec without throwing an error", function() {
      var edisonLib = require('edisonapi');

      var validator = new edisonLib.ServiceSpecValidator();
      validator.readServiceSpecFromFile(path.join(__dirname, "serviceSpecs/temperatureServiceMQTT.json"));
      var spec = validator.getValidatedSpec();

      expect(spec.name).to.be.a('string');
    });
  });
});
