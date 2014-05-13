/**
 * New node file
 */
var expect = require('chai').expect;

describe('edisonlib installation', function () {
    it("tests if edisonapi installed correctly", function() {
      var edisonLib = require('edisonapi');
      expect(edison.sayhello()).to.equal("Hello Edison user!");
    });
});