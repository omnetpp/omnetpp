:author: Levy
:date: 2020-09-17

Zero Configuration Automatic Parallel Simulation
================================================

.. note::
    This is a *Proposed Research Topic*. Proposed research topics are ideas
    that we find both *very promising* as a research topic, and *practically
    very useful*. We have already spent some time trying out the idea and proven
    (at least to ourselves) that it is feasible and the approach outlined here
    can be made to work, but we don't have the resources (mostly, time) to
    elaborate it in-house. If you are a researcher (e.g. PhD student) looking
    for an exciting and rewarding topic to work on, we'd love to hear from you!

Abstract
--------

OMNeT++ already supports parallel simulation. However, the network must be
partitioned manually, and the partitions run concurrently in separate processes
communicating over MPI. Here we propose a parallel simulation approach that
utilizes shared-memory multiprocessors (relies on multi-threaded execution
instead of communicating processes), and requires no prior knowledge or manual
configuration about the simulation model. This makes it potentially both
more efficient and more convenient to use than the existing solution.

Details
-------

The suggested research topic involves the following tasks:

* Change the simulation kernel to support concurrent event execution. Use
  lock-free data structures (e.g. FES) if possible or add locking if necessary.
  Eliminate global state and make the kernel API re-entrant. This part is quite
  difficult to do, because it requires deep knowledge of the simulation kernel.

* Develop a worker thread based approach for executing events concurrently. This
  is fairly straightforward to do, because the required techniques are
  well-known and widely-used.

* Develop a method for determining which future events can be concurrently
  executed. In other words, find the events which have no effect on other events
  in the FES (i.e. out of their light cone).

The idea related to the last point is described in further detail here:

The module structure of the simulation network can be thought of as a graph
where each node corresponds to a module and each edge corresponds to a
connection between the two modules. Other cross-module dependencies, such as
possible C++ method calls or communication via signals, must also be included as
edges.

Each node and edge has a delay associated with it. Node delays are 0 by default
(could be overridden by module), edge delays are either 0 or they are set to the
delay of the corresponding connections. The shortest delay between any two nodes
can be determined by analyzing the paths between the two nodes in the graph.

The FES contains a set of events or most likely messages and each message
belongs to a module. When a module processes a message, the message may have
some effect on other future events in other modules, but this is limited by the
shortest delay between the two modules in question.

The earliest effect time for a message in the FES (i.e. the earliest input time
for the receiver module) can be defined as the minimum of the arrival time plus
the shortest delay (from the other receiver module to this receiver module) for
any other message in the FES. If the earliest effect time for a message is
greater than the arrival time, then the message can be executed concurrently.

The above condition is conservative in the sense that if the FES is investigated
over and over again, then any message that is already found to be concurrently
executable remains to be so. In a large simulation, where the FES contains
several thousands of events, the number of events that can be concurrently
executed can be larger than the number of available CPUs, thus allowing
efficient concurrent execution.

It is also important to note that individual CPUs can grab several concurrently
executable events from the FES at once, and execute them in any order. This
approach can further decrease the contention on the FES when the workers
concurrently access it.

Let's colorize concurrently executable events as green and all other events as
red. All events are red by default, but may be colored green later on. Once an
event becomes green it, remains green until it is executed. The colorizing
algorithm could work as follows::

    for each red event E1 in module M1 in the FES:
        for each event E2 in module M2 before E1 in the FES:
            Minimize arrivalTime(E2) + minimumDelay(M2, M1) as T
        if arrivalTime(E1) < T:
            mark E1 as green

The above algorithm can be run concurrently with the parallel simulation worker
threads, because it is conservative with respect to coloring events.

The minimumDelay between two modules can be predetermined during initialization if
the network topology is static or refreshed if necessary. This data structure
could be quite large. For example, in a simulation containing 1000 nodes there
could be 100,000 modules, so the table would contain 10,000,000,000 rows.

Luckily most of the rows can be merged. For example, if from two modules M1 and
M2 the delay D is the same towards module M3, then the two rows could be
represented by one where the source is the set of M1 and M2. This can be done
similarly when the source is the same and the destinations are different but the
delay is still the same. If a set contains all modules within a compound module
(i.e. the complete module hierarchy), then it can be simply represented by the
compound module.

In an INET simulation, most modules inside a network node are connected with
each other via zero-delay connections and C++ method calls. Thus, the above method
should naturally lead to a data structure where the connections between network
nodes have separate rows and they use the connection delay between the network
nodes. Please note that INET allows combining its modules in many different
ways, so it is not necessarily so trivial. For example, sub-networks can be
represented as extra compound module levels, etc.

Another important optimization opportunity is to use the fact that most
simulations don't allow terminating an ongoing transmission. So if a connection
is used by a transmission, then the next message cannot be sent from the source
to the destination earlier than the end of the ongoing transmission. And due to
the fact that the transmission time is often orders of magnitude larger than the
propagation delay, this could further increase the effectiveness of the above
method.

Current Status
--------------

The above approach has been quickly tested with an INET simulation that consists
of 4 sub-networks each containing an Ethernet switch and a few communicating
hosts, plus some cross-sub-network communication. The result was that the above
approach listed 4-6 concurrent messages out of usually 100 in the FES. This number
may not seem very high, but it would scale linearly with the size of
the FES. That is, in a much larger network with a FES containing several thousands
of events, the number of concurrently executable events can be as large as a few hundred.
This would perhaps allow using all CPUs of a modern computer with almost linear
performance gains.
