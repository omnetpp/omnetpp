RoutingDB
=========

Static shortest-path routing example, using a centralized routing database. 
This is a variation of the "Routing" example simulation.

The routing database is encapsulated into the RoutingDB module which has
a single instance in the model. The Routing modules of all nodes ask the
global RoutingDB to find out the next-hop gate for a packet's destination.

The Routing modules learn the name of the RoutingDB module (more precisely,
its full path) from a parameter. Then they invoke simulation.moduleByPath()
to get a pointer to the module object, and cast it to RoutingDB *. 
Later in the simulation they directly call RoutingDB's nextHopGateId() method
whenever they need routing information. Note the Enter_Method() macro 
at the top of nextHopGateId()'s body -- this enables OMNeT++ to animate the 
method call (if this animation feature is enabled, of course).

The model uses no routing protocol. RoutingDB explores the topology
at the start of the simulation using cTopology, computes all shortest
paths, builds the datase from it, and this information is used
throughout the simulation.

It is interesting to compare this simulation to the "Routing" demo.
It also uses cTopology, but instead of using a central routing
database, nodes discover the topology and build routing tables
independent of each other. Centralized building of the the routing
database reduces the number of shortest path calculations from
N*N to N (N being the number of nodes). The amount of memory needed
to hold routing information is roughly the same in both cases,
although more sophisticated implementations may find ways to "compress"
the centralized routing database. Once the routing database has been
built, there shouldn't be much difference between the execution speeds of the
two simulations.



