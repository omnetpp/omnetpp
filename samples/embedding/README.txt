Embedding
=========

This example shows how to embed the simulation kernel into another program.
The example defines the minimal environment required to execute a simulation.
- The model subdirectory contains all the code and NED files that belong to the
  simulation we want to execute.
- The main.cc file contains example code for embedding the simulation.
  - MinimalEnv: a very basic environment that provides the parameters to
    the simulation. In the current form it accepts all default values
    defined in the NED file (i.e. ini files are not supported at all).
  - simulate() method: contains the network initialization and the main
    simulation loop
  - the main function creates the required environment object and handles
    the initialization and cleanup of the simulation object
