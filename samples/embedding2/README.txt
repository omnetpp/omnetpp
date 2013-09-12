Embedding
=========

This example shows how to embed the simulation kernel into another program.
The example calculates the utilization of an unslotted aloha network based on
the number of hosts in the network and the packet inter-arrival time. Both
parameters should be provided by the user at the beginning of the simulation.

The example defines an environment (MinimalEnv) which provides the input parameters
for the simulation and also stores the result (scalar statistics) in an in-memory map.

The aloha model is contained in the model directory. NED files are embedded in
the executable by using the text2c utility program.

The simulateAloha(simtime_t limit, int numHosts, double iaMean) method sets up
an environment for the simulation run and executes it. The method contains the
initialization code, the main simulation loop and the de-initialization steps.

The main method is used only to set up the network topology by reading the NED files
and then repeatedly call the simulateAloha() method and print out the result.


