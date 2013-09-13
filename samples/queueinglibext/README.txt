QueueingLibExt & Queuinglib
===========================

The QueueingLibExt project demonstrates how to use and extend an already
existing model library (QueueingLib) from a different project. QueueingLibExt
contains some C++ code that extends the behavior of the Sink module from the
QueueingLib project. (The extended SinkExt module will display the number of
received jobs on the GUI.) The external dependency is linked dynamically to the
project executable.
