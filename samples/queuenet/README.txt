Queuinglib
========

QueueNET is a queuing network tutorial and queuing library for OMNeT++.
It is the successor of the Queue, Queueing, Fifo samples from earlier
versions of OMNeT++. It contains several basic building blocks that can be
used to build more complex queueing theory related networks. Jobs are passed
between these blocks.

Basic building blocks:
======================

- Source
  Allows the generation of Jobs. Can limit the start and end-time of the
  generator, the number of Jobs generated, the Job inter-arrival time and
  the Job type and it's priority.

- Sink
  Consumes incomig Jobs and creates statistics from them.

- Classifier
  Classifies all incoming Jobs based on Job type or priority and sends out
  them on the corresponding output gates.

- Delay
  Delays a Job by an amount of time. The dealy can be specified as a random
  distribution.

- Fork
  Creates copies from incoming Jobs and sends them out on ALL output.
  Copies are labelled after the original Job that got copied.

- Join
  Any Job arriving on an INPUT will be sent out immediately on the SINGLE
  output gate.

- PQueue
  A PassiveQueue without processing. Its output MUST be attached to a Server
  to be able to consume Jobs. Queue size can be limited and Jobs over
  the size limit will be dropped (capacity). Queuing strategy can be set 
  eiter to FIFO or LIFO. If more than one server is attached to the PQueue's 
  output it uses the sendingAlgorithm parameter to decide where the newly 
  received Job should be sent. Gathers statistics like max, min, average
  queue length. Average queuing time.

- Server
  A Job processing unit. Only PQueue-s should be attached to its input. If 
  more than one PQueue is attached 'fetchingStrategy' is used to decide which
  queue is queried if the Server becomes idle. 'serviceTime' should be set
  to define how Jobs are processed by the Server.

- Queue
  An active provessing server with a built in queue. Basically it is a PQueue
  and a Server built into a single module.

- Router
  Routes the incoming Jobs using different algorithm like random, roundRobin,
  shortestQueue etc.

========
QueueNET - queuinglib examples
========

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
  and routed to different queues based on user selecion. Operators are 
  processing either one or more queues. After speaking with an operator a
  customer may be re-routed to a different queue.
