Queuing
=======

The model demonstrates a queueing network.

The "source" module generates messages (jobs, parts, etc.) with different
message kind values (0,1,2 and 3 with uniform probability). These messages
get into "classifier" which looks at the message kind and forwards the
message on the corresponding gate (kind=0 on gate out[0], kind=1 on gate
out[1], kind=2 on gate out[2], and the rest (kind=3) on the gate named other.
The `other' messages get discarded in the "drop" module, the others get into
"delay" modules where they each suffer a small constant delay. After the delay
they are queued up in "passiveq" passive queues, served by "qserver".

"qserver" serves the three queues in priority order: passiveq[1] is only
examined if passiveq[0] is empty, and passiveq[2] is only examined if both
passiveq[0] and passiveq[1] are empty. Examination of the queues is done
by calling a method in the passive queue module objects (and not via
message exchange). Once the server decided from which queue to get a
message, it tells the passive queue object to send it (this is also done via
a method call.) The processing time in the server is random. After
processing the messages are forwarded into "sink" where they end their
life circle.
