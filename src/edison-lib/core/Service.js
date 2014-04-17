exports.main = require("../main.njs");

function Service(description) {
  "use strict";
  console.log("created service");
}

// export the class
module.exports = Service;