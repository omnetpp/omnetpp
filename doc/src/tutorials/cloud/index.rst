Using Cloud Computing for |omnet++| Simulations
===============================================

.. rubric:: Introduction

The goal of the tutorial is to give you an insight on how to start harnessing
the power of computing clouds for running simulation campaigns in order to
reduce the time it takes for them to complete. We will focus on using Amazon's
AWS, but the process can be easily ported to other cloud service providers.

.. rubric:: Motivation

Simulation is a CPU-intensive task. A simulation campaign may easily consist of
hundreds or thousands of simulation runs, and can easily exceed the capacity of
computing resources usually available to the researcher.

Nowadays, CPU power is available in abundance in the cloud for anyone, at very
affordable prices. There are numerous cloud computing services (Amazon AWS,
Microsoft Azure, DigitalOcean, Google Cloud Platform, etc.). These services,
following an easy registration, allow the user to run their own code on a high
number of CPUs, at a surprisingly low price. For example, one hour of usage of
an 8-core CPU with 32 GiB RAM costs about $0.50 on AWS at the time of writing.
There is also a free trial, which grants the user one year of CPU time for free.

Simulation campaigns are often trivially parallelizable. Given enough CPUs, the
whole campaign may complete in the time it takes for the longest run to finish.
In this tutorial, we show you how to harness the power of computing clouds to
dramatically speed up your simulation campaigns. The gain will not only save you
time, but it may also allow you to expand the scope or increase the depth of
your research, and come to new discoveries.

.. rubric:: Structure of this Tutorial

In the first part of this tutorial we will explain the basics of cloud services,
get you familiar with the concepts of the most commonly used container
technology, Docker, and introduce job queues.

In the second part, we present a concrete solution that allows you to upload
and execute simulations in AWS. The process should be regarded as a starting
point which illustrates the concept, and can serve as a base for future, more
sophisticated solutions.

.. toctree::
   :caption: Tutorial Contents:
   :maxdepth: 1

   page1
   page2
   page4
   page5
   page6
   page7
