Aloha
=====

Model to measure the throughput of the Aloha and Slotted Aloha protocols.
The model is similar to the original University of Hawaii radio network
hosts talk via a central "server". In this model we're interested in the
achievable channel utilization only, so we ignore the "downstream" link
(TDM broadcast from the server) and retransmissions. Hosts transmit
according to Poisson process, the central server checks for collisions
and computes channel utilization.

In the Aloha protocol when hosts want to talk, they listen first to determine
of there's no undergoing transmission. With the radio nodes scattered around,
the propagation delay of radio signals is different for every host pair
(not to speak of signal attenuation), and it is very complicated (and very
costly, in run-time) to accurately model whether any host "hears" any
transmission at any given moment. Instead, we use a simplification:
if the central server is receiving a transmission, hosts will consider the
channel to be busy, otherwise it's free. This is implemented with the
Host module class calling the isChannelBusy() method of the Server class.
Note that in the model, while packets take time to arrive at the Server
(sendDirect() with radioDelay), Hosts can learn immediately whether
the Server is receiving.


