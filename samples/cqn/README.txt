CQN (Closed Queueing Network)
=============================

The model consists of N tandem queues where each tandem contains
a switch and k single-server queues with exponential service times.
The last queues are looped back to their switches. Each switch
forwards incoming jobs randomly to one of the tandems' first queues,
using uniform distribution. The queues and switches are connected
with links that have nonzero propagation delays. This model wraps
tandems into compound modules.

A fixed number of jobs are created at the beginning of the simulation --
jobs are not created or deleted during simulation.



