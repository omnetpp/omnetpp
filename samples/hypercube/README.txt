HCUBE
=====

Hypercube-interconnected network with simplified deflection routing.
Each node consists of router (rte) module, a source and a sink.

Demonstrates:
  - hypercube topology with dimension as parameter
  - topology templates
  - result collection

Model description
-----------------
A hypercube with d dimensions has 2^d nodes. If we number the nodes
from 0 thru (2^d)-1 and look at the numbers as d-digit binary
numbers, then each node will be connected to d other nodes which differ
only in one bit. That is, suppose d=4 for example, then 0010 (node 2)
will be connected to 1010 (node 10), 0110 (node 6), 0000 (0) and 0011
(node 3). This is how the hypercube is built in hcube.ned; the gate
number is the number of the bit that differs, that is, node 0010's
gate #2 goes to node 0110.

The hypercube module in hcube.ned takes the d dimension as a parameter.
It also takes another parameter called nodetype: a string that names
the module type to be used as node in the hypercube. In hc_net.ned,
when a hypercube network is described, nodetype is give the "HypercubeNode"
value. HypercubeNode is a compound module type which consists of a
traffic source (generator), a traffic sink and a router module.

The router module works in the following way. There are time slots to which
all transmission is synchronized. In each time slot, the router node
receives packets from its neighbors, decides on which gate each packet
needs to be sent, and sends them out in the next time slot.
In a hypercube, routing is simple: the binary numbers of the current
node and the destination node differ in some bits; one bit needs
to be 'corrected' and the packet needs to be sent out thru the corresponding
gate. For example, if we are in node 0010 and the packet's destination
is 1000, then bit 3 and bit 1 differ, thus the packet can be sent out either
on gate 3 or on gate 1. Thus ideally, any packet could reach its destination
in a number of hops that is the Hamming distance between its source and
destination binary node numbers.

The router module uses deflection routing. This means that there is no buffer
in the router module: if more than one packet needs to be sent out thru
the same gate, then one is deflected: it is sent out thru another gate.
Packets from the local user (traffic source or generator) can only be
accepted if there is a free gate left in the given time slot, that is,
less than d packets need to be routed. The hc_rte.cc file contains the
HCRouter simple module which implements a simple deflection routing
scheme.
