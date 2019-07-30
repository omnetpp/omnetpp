Parallel execution of CQN
=========================

This example demonstrates the parallel distributed simulation capabilities
of OMNeT++. The model itself is the "stock" CQN simulation -- it hasn't been
modified or specially instrumented for parallel execution, it "only"
has the properties that make it a good candidate for parallel execution.
(More about this later.) The cqn/parsim directory doesn't even contain
a simulation executable, only configuration (omnetpp.ini) and a script
(bat) file -- the program itself is the cqn executable.

The three tandem queues in the CQN example are executed in three separate
processes. When you launch the simulation (runparsim), you should see 3
Qtenv windows pop up.

********************
IMPORTANT: You need to click Run in all 3 Qtenv windows to start the simulation!
********************

Clearly, if you want to measure performance, you'll choose "Express Run" in
all three processes. If you have multiple CPUs, chances are that the OS will
assign the 3 processes to different CPUs, so you might be able get speedup.
(Overhead from inter-process communication and the synchronization protocol
will eat some of the performance.) According to the default omnetpp.ini
settings, the processes communicate with each other via named pipes. This can
be switched over to MPI (for performance and clustering) or to temporary files
(if you want to see what messages are exchanged). Of course, for "production"
use you'll want to switch over from GUI (Qtenv) to the command-line interface
(Cmdenv).

The synchronization mechanism is by default the conservative null message
algorithm; OMNeT++ makes it possible to implement and plug in others, and
select them from omnetpp.ini. To demonstrate why you need a synchronization
mechanism, you can select the "no synchronization" synchronization mechanism
and see what happens. Partitioning of the model is also given in omnetpp.ini,
and as such it can be easily changed. (For example, you can run a 12-tandem
CQN in 2, 3, 4, 6 or more processes.)

What makes the CQN simulation easy to run in parallel is that it can easily
be partitioned in a way such that the partitions are connected with long-delay
links. Link delays provide lookahead for the conservative synchronization
algorithm, and good lookahead (long delays) is crucial to get an acceptable
speedup (or any speedup at all). You can easily verify this: lookahead is
defined by the "*.sDelay=100" parameter setting in omnetpp.ini. If you
decrease this 100 to smaller values, at one point you'll experience a sharp
drop in performance (ev/sec or simsec/sec values).

The bad news is that usually link delays should be quite high (and/or
communication delays between CPUs should be very small) to get good speedup,
so odds are rather poor that you'll be able to use the parallel simulation
feature in practice to speed up your own simulations (or any other sample
simulation here). You've been warned!

You can find further info on the (quantifyable) relationship among lookahead,
communication delay (among CPUs, i.e. the MPI delay), computational intensity
of processing one event, and other factors in the publications on our parallel
simulation research website,
http://ctieware.eng.monash.edu.au/twiki/bin/view/Simulation/ParallelSimulation

