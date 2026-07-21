Architecture and Implementation Details
=======================================

This is a distributed application, built on top of `Docker Swarm <https://docs.docker.com/engine/swarm/>`_
(the new, integrated "Swarm Mode", not the legacy docker-swarm utility).
It is composed of seven different Docker `services <https://docs.docker.com/engine/swarm/how-swarm-mode-works/services/>`_.


Services
--------

The application is made up of several Docker services:
redis, mongo, builder, runner, visualizer, dashboard, distcc.
Let's discuss what each of them are there for, and what they do.

Redis
~~~~~

This service runs the official Redis image on the Manager. It is needed by RQ (Redis Queue)
that we use for job queueing.

Mongo
~~~~~

Mongo is a database that we use for temporary storage of binaries and result files.

Builder
~~~~~~~

This is one of the two services running an RQ worker. It starts a single
container on the manager, and listens on the ``build`` queue for jobs.

It uses the distcc servers from the ``distcc`` service through the ``buildnet``
network to distribute the compilation tasks across all nodes.

Once a build is done, it submits the binaries (actually, the ``libINET.so`` file) to the Mongo service,
so the runner containers can access it later.

Runner
~~~~~~

The other RQ worker, running as many containers in each host, as their respecrtive number
of cpu cores. Except the manager, because that needs some extra juice running the other services,
like redis and mongo. it is done by requesting a large number of containers (100), but reserving
95% of a core for each container, so they automatically "expand" to "fill" the available number
of (remaining) CPUs, like a liquid.

It gets the built libINET.so from the MongoDB server, and also submits the simulation results there.

Visualizer
~~~~~~~~~~

This service starts a single container on the manager, using
the official `docker-swarm-visualizer <https://github.com/dockersamples/docker-swarm-visualizer>`_ image.
In that container, a web server runs that lets you quickly inspect
the state of the swarm, including its nodes, services and containers,
just using your web browser. It listens on port ``8080``, so once
your swarm application is up and running (and you are connected to the swarm
if it is on AWS), you can check it out at http://localhost:8080/.

Dashboard
~~~~~~~~~

Similarly to the ``visualizer``, this is an auxiliary service, running a single
container on the manager, with a web server in it.
This one lets you see, and manage in a limited way, the RQ queues, workers, and jobs.
See: http://localhost:9181/.

distcc
~~~~~~

This service starts exactly one container on all nodes (workers and the manager alike).
They all run a distcc server, listening for incoming requests for compilation (completely
independent from RQ).
They are only attached to the ``buildnet`` network, and have deterministic IP addresses.

When the ``builder`` container starts a build in a ``build`` job, it will try
to connect to the ``distcc`` containers, and will use them to distribute
the compilation tasks to all nodes.

Networks
--------

The stack also contains two virtual networks. Each service is attached to
one or both of these networks. The networks are:

- interlink
- buildnet

Both of them use the ``overlay`` driver, meaning that these are entirely virtual
networks, not interfering with the underlying real one between the nodes.

Interlink
~~~~~~~~~

This is the main network, all services except ``distcc`` are attached to it.

Buildnet
~~~~~~~~

The ``buildnet`` network connects the containers of the ``distcc`` service
with the ``builder`` service. It operates on a fixed subnet, The ``builder``

This was only necessary to give the ``distcc`` containers deterministic and known
IP addresses. On ``interlink`` they didn't always get the same addresses, they
were randomly interleaved with the containers of all the other services.

This would not be necessary at all if multicast traffic worked on ``overlay``
networks between nodes, because then we could just use the built-in zeroconf
service discovery capabilities of distcc (the software itself). However, until
`this issue <https://github.com/docker/libnetwork/issues/552>`_ is resolved, we
have to resort to this solution.


Operation
---------

aws_swarm_tool init:

Deploying the official CloudFormation template supplied by Docker, called Docker for AWS.

Using the default settings, the script creates 1 manager and 3 workers, each of them as a ``c4.4xlarge`` type Instance.

It also creates an alarm and an AutoScaling policy that makes sure that all machines are shut down after 1 hour of inactivity (precisely, if the maximum CPU utilization of the manager machine was below 10 percent for 4 consecutive 15 minute periods).
This is to reduce the chances that they are forgotten about, and left running indefinitely, generating unexpected expenditure.

To be able to connect to the Swarm we are about to create, we must first create and SSH keypair.
The ``aws_swarm_tool.py`` can do this for us.

Connecting to the Swarm is essentially opening an SSH connection to the manager machine, and forwarding
a handful of ports through that tunnel from the local machine to the swarm.
There is no need to do it manually, the ``aws_swarm_tool.py`` script has a command for it:

.. code-block:: bash

    $ aws_swarm_tool.py connect

In addition to bringing up the SSH connection, the script also saves the process ID (PID) of the SSH client process into a file (so called PID-file) in a temporary directory (most likely ``/tmp``), called


What is Docker Swarm?
~~~~~~~~~~~~~~~~~~~~~

With version 1.12.0, Docker introduced *Swarm Mode*. It makes it possible to
connect multiple computers (called hosts or nodes) on a network, into a cluster - called swarm.

This new feature enables someting called "container orchestration". It makes the development, deployment,
and maintenance of distributed, multi-container applications easier. You can read
more about it `here <https://docs.docker.com/engine/swarm/>`_.

One great advantage of this is that wherever a Docker Swarm is configured, any
application can be run, let it be on local machines, or any cloud computing platform.
