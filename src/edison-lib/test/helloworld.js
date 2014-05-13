/**
 * New node file
 */
var expect = require('chai').expect;

describe('[edisonlib installation]', function () {
  it("should verify if edison library is correctly installed", function() {
    var edisonLib = require('edisonapi');
    expect(edisonLib.sayhello()).to.equal("Hello Edison user!");
  });
});