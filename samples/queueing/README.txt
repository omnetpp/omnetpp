Queuing
=======

The model demonstrates a queueing network. While the model is an abstract
one, the following more-or-less real-life analogy might help making sense
of it:

  Passengers arrive at the airport building at random intervals, and they want
  to check in at a particular counter. They first go to the information desk.
  There it turns out that some passengers should have gone to a different
  terminal, so they leave the building. The others have to walk along a long
  corridor to get to the check-in, where there're different queues for economy
  class, business class, first class etc. passengers. Unfortunately there's
  only one clark. She serves the queues in priority order -- economy class
  passengers are only served if there's no business class or first class
  passenger. After check-in the passengers get into the duty-free zone and
  have a good time until boarding.

In the model, the "enter" module generates messages with different message
types (message kinds 0,1,2 and 3 with uniform probability). These messages
get into "classifier" which looks at the message kind and forwards the
message on the corresponding gate (kind=0 on gate "out[0]", kind=1 on gate
"out[1]", kind=2 on gate "out[2]", and the rest (kind=3) on the gate "other".
The latter messages get discarded in the "leave1" module, the others get into
"delay" modules where they each suffer a small constant delay. After the delay
they are queued up in "passiveq" passive queues, served by "qserver".

"qserver" serves the three queues in priority order: "passiveq[1]" is only
examined when "passiveq[0]" is empty, and "passiveq[2]" is only examined when
both "passiveq[0]" and "passiveq[1]" are empty. Examination of the queues
is programmed by calling a method in the passive queue module objects
(as opposed to message exchange, or integrating all queues and the server
into a single module). Once the "qserver" decided from which queue to obtain
a message, it tells the passive queue object to send it (this is also done via
a method call.) The processing time in the server is random. After processing,
messages are forwarded into "leave2" where they end their life circles.

In "leave2", statistics are collected about the time messages spent in the
system. These statistics can be viewed during simulation (open the inspector
window for "leave2", then click the "Contents" tab and open inspectors for the
statistics objects), or after simulation (open omnetpp.vec in Plove).


