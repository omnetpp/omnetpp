Sockets (Hardware-in-the Loop Simulation Demo)
==============================================

This is an example how one can implement hardware-in-the-loop simulation
with OMNeT++/OMNEST. Hardware-in-the-loop means that some external device
is taking part in the simulation. (In other scenarios, the simulation may
implement a device in a real system).

This example contains two simulation models:
 - HTTPNet models a network where

The word "real-time" means that the simulation is executed in a way such that
simulation time is synchronized to real time: simulating one second
in the model will also take one "real" second. This is in contrast to
"normal" simulation, when we want the simulation to run as fast as possible.
Of course, the CPU has to be fast enough so that the simulation can keep up
with real time.

Hardware-in-the-loop simulation is enabled by the following omnetpp.ini entry:

[General]
scheduler-class = "cSocketRTScheduler"

When you run the simulation, set animation speed to near maximum (using the
slider at the top of the graphics window) for the simulation to be able to
keep up with real time (you can check on the performance gauge bar:
the simsec/sec gauge should stay around 1.0).

