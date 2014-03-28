module.exports =
{
	libRoot: __dirname,
	interfaceDir: "plugin-interfaces",
	interfaceImplDir: "plugins",
	superInterfaceName: "interface-for-interfaces",
	serviceDir: "serviceSpecs",
	components: {
		localDirectory: "edison-mdns.js",
		localComm: "edison-mqtt.js",
		cloudComm: "",
		cloudDirectory: ""
	}
};