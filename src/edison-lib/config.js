module.exports =
{
		libRoot: __dirname,
		pluginInterfaceDir: "plugin-interfaces",
		pluginDir: "plugins",
		superInterfaceName: "interface-for-interfaces",
		serviceDir: "serviceSpecs",
		components: {
			communication: {
				plugins:
					[
					 	{fileName: "edison-zmq.js"}
					]
			}
		}
};
