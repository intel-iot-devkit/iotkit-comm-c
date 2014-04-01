module.exports =
{
		libRoot: __dirname,
		pluginInterfaceDir: "plugin-interfaces",
		pluginDir: "plugins",
		superInterfaceName: "interface-for-interfaces",
		serviceDir: "serviceSpecs",
		components: {
			localDirectory: {
				plugins:
					[
					 	{fileName: "edison-mdns.js"}
					]
			},
			localComm: {
				plugins:
					[
					 	{fileName: "edison-mqtt.js"}
					]
			},
			cloudComm: {
				plugins:
					[
					 	{fileName: "edison-mqtt.js"}
					]
			},
			cloudDirectory: [{}]
		}
};
