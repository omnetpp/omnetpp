Routing/Networks
================

Demonstrates dynamic loading of NED files via the Routing sample simulation.
This demo will only work if OMNeT++ was compiled with support for dynamic
NED loading (WITH_NETBUILDER option).

The point is that you can test routing on any topology without nedtool or C++
compilation. The NED file containing the compound module and network
definition simply has to be added to the "preload-ned-files=" omnetpp.ini
option, and that's all -- dynamically loaded NED files act appear like
the ones compiled into the simulation program.

