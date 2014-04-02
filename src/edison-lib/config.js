module.exports =
{
		libRoot: __dirname,
		pluginInterfaceDir: "plugin-interfaces",
		pluginDir: "plugins",
		superInterfaceName: "interface-for-interfaces",
		serviceDir: "serviceSpecs",
		components: {
			discovery: {
				plugins:
					[
					 	{fileName: "edison-mdns.js"}
					]
			},
			communication: {
				plugins:
					[
					 	{fileName: "edison-mqtt.js"}
					]
			}
		}
};
