QueueingLib
===========

QueueingLib is an example process modeling library for queueing and resource
reservation models. It contains several building blocks that can be used to
build more complex queueing networks. The library allows you to build your
initial performance model quickly, and experiment with it to get an initial
idea about your system's performance. This performance model can then serve
as a starting point for the development of more detailed, more accurate, more
specialized simulation models. The code is compiled into a shared library
that can be used from other projects.

All library blocks expose statistics in the form of simulation signals, which
can be recorded as time series data (timestamp - value pairs) or as summaries
(mean, sum, maximum value, histogram, etc.) Statistics collection can be
controlled on a fine-grained level.

This directory contains the queuing library implementation; see the QueueNet
folder for example usage.


The library contains the following blocks (modules):

Source, Sink

    A Source block generates jobs for the queueing network. Generation can be
    one-off or periodic, where the generation interval is parametric. A Source
    block can be used to model constant-rate arrivals, Poisson arrivals, or
    arrivals with an inter-arrival time of some other distribution.

    Sink represents places where jobs leave the system, and also collects
    statistics.

Delay

    A Delay block delays the job for an interval, which can be constant or
    given with a probability distribution. Delay can be thought of as a queue
    with an infinite number of servers.

Queue

    A Queue block represents a queue with a built-in server. The queue
    discipline can be FIFO or LIFO, and the queue can be infinite or bounded.
    The service time can be constant, or random according to a user-specified
    probability distribution.

PassiveQueue, Server

    PassiveQueue and Server represent a detached queue and queue server. They
    support one-to-many, many-to-one and many-to-many configurations: one
    server may serve more than one queues, and one queue may be served by more
    than one servers. Several conflict resolution disciplines are available for
    both queues and servers, random, round-robin and priority-based among
    others.

Fork, Join, JobList

    The Fork and Join blocks can be used to model splitting a job to sub-jobs.
    For each incoming job, Fork creates N sub-jobs which are sent out on
    different outputs. These sub-jobs may be processed individually (in Queue
    blocks, etc.), and should end up in the same Join block. After all sub-jobs
    have arrived in the Join block, it sends out the original job and discards
    the sub-jobs. The JobList block presents a view of all jobs and sub-jobs in
    the system. Fork-Join blocks may be nested, i.e. a sub-job may be split
    further by subsequent Fork blocks.

ResourceBasedQueue, ResourcePool

    A ResourceBasedQueue is a queue with a built-in server that needs to
    acquire an external resource from a ResourcePool for processing a job. When
    the server is done with the processing, it returns the resource to the
    ResourcePool. A ResourcePool holds a configurable amount of resources, and
    the amount of resources a queue server needs for processing a job is also
    configurable. ResourcePool implements priority-based conflict resolution.

Allocate, Deallocate

    Allocate and Deallocate can be used to acquire resources for a series of
    processing steps. Allocate acquires a given amount of resources from a
    ResourcePool for each job, and Deallocate returns them into the
    ResourcePool. Jobs that arrive at an Allocate block when there aren't
    sufficient resources available in the ResourcePool are queued.

Router, Classifier

    Router and Classifier can be used to route or classify jobs, based either
    on some algorithm (random, round-robin, etc.) or an attribute of the job
    (priority, type, etc.)

Clone, etc.

    Additional library blocks perform further tasks, like the Clone block that
    duplicates every incoming job to each of its outputs.


Naturally, not all models can be expressed with the above set of blocks, i.e.
the library is not Turing-complete. Turing-completeness comes from the fact
that all blocks are implemented in C++ with the source available for
inspection and modification, so you can build in the extra functionality you
need.


