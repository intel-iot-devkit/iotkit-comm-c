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
					 	{core: true, fileName: "edison-mdns.js"},
					 	{ignored: true}
					]
			},
			localComm: {
				plugins:
					[
					 	{core: true, fileName: "edison-mqtt.js"}
					]
			},
			cloudComm: [{}],
			cloudDirectory: [{}]
		}
};