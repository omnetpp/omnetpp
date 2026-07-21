Running INET Simulations on AWS
===============================

.. rubric:: Introduction

This tutorial will show you how to utilize AWS, Amazon's cloud computing service,
for running INET simulation campaigns. Following registration and minimal configuration
on the AWS management interface, you'll be able to use a command-line tool 
for running simulations on AWS much like you'd run simulations locally. Our tool
transparently takes care of submitting the simulation jobs to AWS, orchestrates 
their execution, and downloads the result files into the same local ``results/``
folder where the locally running simulation would create them.

Your simulation project needs to be published on GitHub, because AWS cloud
nodes retrieve the source code of the simulation by checking it out from GitHub.
The tool currently has some hardcoded assumptions about the project, so it only 
works with INET and INET forks. Work is underway to make the tool generic
enough to run any simulation.

Running the simulation on a cloud service incurs some overhead (retrieving the source of the
simulation on cloud nodes, building it, distributing the binaries to all processors,
and finally downloading the results to your own computer), so your simulation campaign
needs to be large enough to benefit from cloud computing: it needs to consist of 
several simulation runs, and each run should be longer than at least a couple seconds
(real time).

Although AWS offers a Free Tier for trial purposes, the requirements for running
INET simulations unfortunately exceeds the resource constraints of the Free Tier.
However, AWS usage is very affordable (expect prices of around 1 USD for one hour of uptime),
so it is still well worth it if you have simulation campaigns that take too
long to complete on your locally available computing resources. 
(We are not affiliated with Amazon.)

This solution utilizes Docker Swarm, and a couple of other services and technologies.
In addition to scripts that manage the tasks closely associated with running simulations
remotely, we also provide a command-line tool to make the management of the Swarm on AWS
easier.

.. toctree::
   :maxdepth: 1
   
   page1
   page2
