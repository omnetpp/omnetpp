DYNA in Real-Time
=================

This demonstrates real-time simulation capability of OMNeT++/OMNEST. 
The word simply means that the simulation is executed in a way such that 
simulation time is synchronized to real time: simulating one second 
in the model will also take one "real" second. This is in contrast to 
"normal" simulation, when we want the simulation to run as fast as possible.

Synchronization to real-time is achieved by usleep() calls of the appropriate 
lengths in the simulation kernel. For example, if the next event is 1s
(simulation time) away, the simulation process will sleep 1s before 
proceeding to execute that event (or rather, it'll sleep somewhat less 
than 1s, maybe 999.6ms, because processing events also takes CPU cycles).

Of course, real-time simulation has its limits. Most important, the CPU has
to be fast enough, and event density (events per simulated second) 
should be low enough so that the simulation can keep up with real time
(don't expect to be able to run cell-level ATM simulations in real-time!)
Also, events that correspond to the same simulation time in the model
will necessarily be executed one after another, so some "jitter" or 
temporary skew in the synchronization will be inevitable.

This example runs the Dyna model in real-time mode. Real-time mode is enabled
by the following omnetpp.ini entry:

[General]
scheduler-class = "cRealTimeScheduler"

When you run the simulation, set animation speed to the maximum (using the
slider at the top of the graphics window), because otherwise the simulation
won't be able to keep up with real time.
