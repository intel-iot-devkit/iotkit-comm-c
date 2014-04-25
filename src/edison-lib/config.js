module.exports =
{
  libRoot: __dirname,
  pluginInterfaceDir: "plugin-interfaces",
  pluginDir: "plugins",
  superInterfaceName: "interface-for-interfaces",
  pluginFileSuffixes: {
    clientFileSuffix: "client",
    serverFileSuffix: "service"
  },
  communicationPlugins:
    [
      {pluginName: "zmqpubsub"},
      {pluginName: "mqtt"},
      {pluginName: "zmqreqrep"}
    ]
};
