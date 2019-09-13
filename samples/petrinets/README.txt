PetriNets
=========

This is a simulation model for stochastic Petri nets, a well-known formalism
for the description of distributed systems. Petri nets can be assembled from
modules using the NED language, or imported from PNML (Petri Net Markup Language)
files. The latter requires a builder module to be added to the simulation that
reads the PNML file and sets up the network at the beginning of the simulation.
A wizard for the OMNeT++ IDE is also provided, which can convert PNML files directly
to NED, eliminating the need for the builder module. An alternative way to
convert PNML files to NED is to copy and paste the log output of the builder
module, since it dumps the equivalent NED source while building.

Most configurations in omnetpp.ini use PNML files as input, and only the
first few are defined using NED. Example PNML files have been borrowed from
WoPeD (https://woped.dhbw-karlsruhe.de), an open-source tool for working with
Petri nets.

A Petri net model is composed of Arc and Transition simple modules. Timed
transitions (where firing takes a finite amount of time instead of being
instantaneous) are also supported. Since the firing order of armed transitions
is nondeterministic due to the Petri net rules and OMNeT++ simulation kernel
does not support such nondeterministic scheduling out of the box, a separate
scheduler module is employed throughout the simulation which determines
(using random numbers) which transition should fire next. (This scheduler,
despite the slightly confusing name, is an entirely normal OMNeT++ simple
module, and not an OMNeT++ event scheduler class derived from cScheduler.)
The scheduler is used from other modules via C++ function calls.

Animation in this model is entirely custom made, and heavily utilizes
the Canvas API and refreshDisplay().
