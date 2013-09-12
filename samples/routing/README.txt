Routing
=======

Demonstrates static shortest-path routing. Routing tables are set up at the
beginning of the simulation using the cTopology class. The model is
intentionally kept simple to facilitate understanding.

The network topology is the one widely known as the "NTT backbone", and it was
contributed by Mudassar Farooq.

Every node queries the topology of the network independently, using a cTopology
object. Then it computes shortest paths to every other node, and stores the
first nodes of the paths in a next-hop table. (Actually the table contains
the port number to the next-hop node not the node address itself -- the table
thus provides dest-address -> next-hop-address mapping). All the above takes
place once, at the beginning of this simulation. The topology is static during
the simulation, and so there's no need for the nodes to do anything to keep
the tables up-to-date. There's no routing protocol in the model.

Once the routing tables are set up, nodes start sending packets at random
intervals. Every node gets a list of destination addresses in a parameter,
and for every packet it randomly chooses a destination from the list.

There are two apps provided: App generates packets with exponential inter-arrival
times, while BurstyApp alternates between active and idle periods. BurstyApp's
implementation demonstrates the use of the FSMs (Finite State Machine).
