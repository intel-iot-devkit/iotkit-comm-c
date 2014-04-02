exports.components = ["communication"];
exports.name = "edisonZmq";
exports.type = "pubsub";

var zeromq = require('zmq');
var client;

exports.createPubClient = function (ip, port, callback) {

    console.log(__filename + ":IP:" + ip + ":port:" + port);

    client = zeromq.socket('pub');
    client.bind('tcp://*:' + port, function (error) {
        if (error) {
            console.log(__filename + " Error: " + error);
        }
        console.log(__filename + "Bind done");
        callback(client);
    });
};

exports.createClient = function (ip, port) {

    console.log(__filename + ":IP:" + ip + ":port:" + port);

    client = zeromq.socket('sub');
    client.connect('tcp://' + ip + ':' + port);

    return client;
};

exports.publish = function (topic, message) {
    client.send(topic + ' ' + message);
};

exports.subscribe = function (topic, callback) {
    client.subscribe(topic);

    client.on('message', function (message) {
        console.log(__filename, "sub received:" + topic + message);
        callback(topic, message);
    });
};

exports.close = function () {
    client.close();
};