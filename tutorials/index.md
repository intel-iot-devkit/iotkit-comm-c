
[Start Here](@ref start-here.md) to begin developing applications and plugins that use iotkit-comm connectivity library. Otherwise, continue
reading to learn more.

iotkit-comm allows network-connected devices to conveniently discover and communicate with each other and the cloud. More specifically,
iotkit-comm library enables developers to write distributed applications composed of clients and servers.
This library was designed primarily for Intel® Edison platform, but works well on other platforms too. iotkit-comm
comes in two flavors: C and node.js. This documentation focuses on the C version of the library.

<B> How to read this documentation </B>

&ensp;&ensp;&bull;  For browsing the library and plugins:<BR>

&ensp;&ensp;    1) iotkit-comm<BR>
&ensp;&ensp;    2) iotkitpubsub<BR>
&ensp;&ensp;    3) enableiot<BR>
&ensp;&ensp;    4) mqttpubsub<BR>
&ensp;&ensp;    5) zmqpubsub<BR>
&ensp;&ensp;    6) zmqreqrep<BR><BR>
&ensp;&ensp;&bull;  For tutorials on how to use the API, the entry point is the 'Tutorials' section (we suggest starting with the ['Start Here'](@ref start-here.md) tutorial).<BR>

<I>Note: For the purposes of this documentation, a module is a logical grouping of functions and variables. Please be aware
that this is the <B> logical structure of the documentation only</B> and not iotkit-comm codebase.</I>

<B> Prerequisites </B> <BR>

This document assumes that: <BR>

1. Your Edison has Internet connectivity <BR>
2. The included sample programs and commands are run on Edison <BR>
