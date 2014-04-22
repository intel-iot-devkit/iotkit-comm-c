module.exports =
{
		libRoot: __dirname,
		pluginInterfaceDir: "plugin-interfaces",
		pluginDir: "plugins",
		superInterfaceName: "interface-for-interfaces",
		components: {
			communication: {
				plugins:
					[
					 	{fileName: "edison-zmqpubsub.js"},
            {fileName: "edison-mqtt.js"}
					]
			}
		}
};
