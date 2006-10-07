Token Ring
==========

Model of a Token Ring LAN.

Demonstrates:
  - ring topology with the number of nodes as parameter
  - the activity() function
  - using cQueue, implementing finite queue size, queue statistics
  - packet encapsulation (encapsulate()/decapsulate())
  - WATCHing variables
  - statistics and output vectors


About Token Ring in general

Token Ring is a LAN standard described in IEEE 802.5. Token Ring uses
a ring topology where the data is sent from one machine to the next
and so on around the ring until it ends up back where it started.
It uses a token passing protocol, which means that a machine can only use
the network when it has control of the Token. This ensures that there are no
collisions because only one machine can use the network at any given time.

At the start, a free Token is circulating on the ring. To use the network,
a machine first has to capture the free Token. Then it can transmit messages
until the Token Holding Time (THT) exprires, then it shuld release the Token
by sending it to the next station.

From a cabling point of view, Token Ring is a star topology network,
with a hub in the middle. The hub simply changes the topology from a physical
ring to a star-wired ring, but the Token still circulates around the network
and is still controlled in the same manner. However, using a hub or a switch
greatly improves reliability because the hub can automatically bypass any ports
that are disconnected or have a cabling fault.


About this model

This is a simplified model of the Token Ring protocol. The ring self-
maintenance part of the protocol (like the negotiation to decide which station
will be become the Active Monitor, and the ring monitoring procedure) are
not modeled. In this simulation, THT is a parameter to the model.

Each station in the ring consists of a MAC module which implements
the protocol, and a source and a sink module which generate and consume
modelled traffic. Token Ring frames are described in token.msg; cMessage's
encapsulate()/decapsulate() methods are used to add payload to Token Ring
data frames.

The MAC module uses a finite size queue to buffer data packets until they
are sent on the network. When the buffer overflows, new packets are simply
discarded (dropped). The MAC module collects statistics about packet
drops.

Other statistics collected include queue lengths in packets and in
bits and end-to-end delays.


The Empty Ring Paradox

An interesting property of the model is that the simulation is very slow
(model time progresses very slowly) when there's little or no traffic
on the ring. This is because the Token is very small compared to data
frames and it circulates very fast on an empty ring, thus it requires
a large number of events per simulated second to simulate it properly.
Some simulators solve this paradox and accelerate the simulation
by "extracting" the Token from the ring when the ring is empty, and
"reinserting" it at a calculated position before there's a new transmission.
To keep the model straightforward and easy to understand, we refrained
from using this trick here.


Running and exploring the model

* Running the simulation
Click on the Run icon on the main toolbar. You'll see the Token and data 
frames circling on the network graphics, and trace output scrolling
in the main window. You can used the object tree on the left side of
the main window to explore the internal structure if the model.

* Exploring the MAC protocol.
Open a message window for a MAC module. Right-click on the icon, then
choose "module output" from the popup. (You can also use Trace|Module output...
in the main menu.) A module output window appears.
Single-step the MAC module, using the icons at the top of the message window.
At each event, the MAC module reports what it is doing.

* Viewing statistics during execution
Open an inspector window for a queuing time output vector. You can find
the corresponding object in the object tree in the main window, opening
the following nodes: "token" --> "comp[1]" --> "sink" --> "local-variables".
Double-click on it to open an inspector, then run the model to collect
data to display (choose "Fast" or "Express" on the main toolbar).
The window will display a graph of the new values written to the output
vector. The drawing style and the axes can be customised.

* Viewing results after the simulation completed
The simulation creates .vec files as output. You can use the Plove program
to plot contents of these files.

* Playing with different network sizes and load conditions.
Parameters of the model are described in the omnetpp.ini file.
You can try out the predefined simulation runs by choosing
the "Set up a run" icon on the main toolbar. By editing
omnetpp.ini, you can also create new runs or alter the parameters
of existing ones, the see the effect of your changes by running the
simulation. For example, if you decrease the "ia_time" parameter of
the generator modules, network offered load will increase, and
when it exceeds the network capacity, the number of drops will
dramatically increase.
