QueueNet
========

The QueueNet project demonstrates how to use an already existing model library
(QueuingLib) from a different project. QueueNet contains no C++ code and
uses only the building blocks (simple modules) provided by QueuingLib.
The "opp_run -l <sharedlibname>" command can be used to start simulations
where code is contained in shared libraries.

QueueNet contains the following example simulations:

- A simple source, server, sink example.
  It is a very simple example showing how to generate, process and consume
  Jobs in queueing network.

- Tandem queues
  Several servers connected to each other in a row.

- A closed queuing network (ring)
  Several servers connected in a ring. Demonstrates how to inject Jobs into
  a queue.

- Airport Terminal
  An example modeling an Airport Terminal, information desk, check-in counter.

- Call Center
  An example demonstrating a call center where incoming calls are categorized
  and routed to different queues based on user selection. Operators are
  processing either one or more queues. After speaking with an operator a
  customer may be re-routed to a different queue.
