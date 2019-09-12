Aloha
=====

This model makes it possible to measure the efficiency of the Aloha and
slotted Aloha protocols. The model is similar to the original University
of Hawaii radio network: "hosts" talk to a central "server" via a shared
radio channel. In this model we're only interested in the achievable channel
utilization, so we ignore the "downstream" link (TDM broadcast from
the server) and retransmissions.

Hosts transmit according to Poisson process. The protocol choice (slotted or
"pure" Aloha) is encode in the slotTime parameter of the host model --
if a nonzero slotTime is given, transmissions are aligned to the next slot
boundary.

The model of the central server checks for collisions and computes statistics.
The internal state of the server and all statistics are visible in the GUI
The most important statistics is channel utilization. It is also recorded in
omnetpp.vec, and can be plotted using the Analysis Tool in the IDE.

The supplied omnetpp.ini file contains 6 predefined configurations:
- pure Aloha at high load
- pure Aloha at moderate load (utilization =~ max)
- pure Aloha at low load
- slotted Aloha at high load
- slotted Aloha at moderate load (utilization =~ max)
- slotted Aloha at low load

In addition, it also contains a predefined parameter study called
PureAlohaExperiment, which explores the channel utilization in the
function of the number of hosts and the packet generation frequency.

According to some descriptions of the Aloha protocol, hosts have to listen
on the channel before they start to transmit anything. This "listen before
talk" behavior is not part of the current model. (One reason is that with
the radio nodes scattered around, the propagation delays of radio signals
are different for every host pair, and it becomes very complex to accurately
model whether any host can hear any transmission at any given moment.)

