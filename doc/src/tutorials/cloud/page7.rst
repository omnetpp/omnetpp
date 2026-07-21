Potential Improvements, Alternatives
====================================

Keep in mind that the main point of this tutorial was to present a very simple
solution. This means that there are some significant compromises we had to make.
In this last section, we discuss a few of them, as well some alternatives to
parts of the system.

Limitations
-----------

Some deficiencies of the current solution, with suggestions on how to alleviate
them:

- It only works well with simple models (like the ones in the ``samples``
  folder). It has no concept of any project features or referenced projects.
  Advanced use cases, for example ones involving multiple processes (like with
  Veins), are not supported. Some custom adjustments to the worker function are
  necessary to make these kinds of simulations possible.

- Model sources are always distributed as a whole. This is not well suited for
  quick iteration when experimenting with the code or with parameter values,
  since we can't take advantage of incremental building. This also generates
  more network traffic, which means longer startup times with large models.

  If the model is already in an online repository (GitHub or similar), the
  workers could be set up to pull a specific revision from there before each
  run. To avoid having to push the code in that repository after each change,
  a local git server can be started as well.

  To avoid even having to commit the changes into git before each iteration,
  the code can be synchronized to the workers using something like ``rsync``, or
  shared with them via a network file system, for example ``sshfs`` or ``nfs``.

- The model is built before every run. Even with ``ccache``, every model is built
  from scratch in every worker container at least once, and the linking phase
  still happens before every run.

  The ideal solution would be building just once, either in a different kind of
  job on one of the workers, or on the local machine. Then distributing the
  built model among the workers, where they are cached locally, and shared
  among containers running on the same host (using a common volume), so they
  only pass through the network as many times as absolutely necessary.

- The client script might not be the most convenient to use. It could be useful
  to extend it with some more options, or even integrate it into the IDE.

- There is no error handling or logging to speak of, the robustness is
  questionable, and we paid no attention to security at all. These are
  relatively significant omissions.

- On multi-core worker machines, multiple worker containers need to be started
  to take full advantage of their capabilities, since currently a worker only
  performs one run at a time. This can be a good or a bad thing, depending on
  your needs. This way, there is more control over how much resources the
  system is allowed to use, but makes the overall picture a bit unwieldy.

- The way the model is passed to the jobs and the results are retrieved is not
  optimal. All data in both directions is stored in the Redis server operating
  the job queue. Since Redis is an in-memory database, this places a limit on
  the overall scalability of the solution, mostly on the size of the results.
  This is why vector recording and event logging is recommended to be turned
  off for now, at least for large simulations.

  A good solution for this would be using a dedicated storage space, accessible
  both by the client and the workers. On AWS, S3 (Simple Storage Service) is a
  promising candidate. Other cloud providers also have similar data storage
  services. Additionally, any of the options noted above for sharing the code
  while iterating can be used for result retrieval as well.

- The progress and console output of the runs are currently not reported at
  all, not while they are under execution, nor afterward. Real-time monitoring
  would be useful, and it can be implemented probably the most easily through
  the already available Redis server.

Alternatives
------------

Many parts of the presented architecture can be swapped out for alternatives. A
few examples:

- Instead of Docker Hub, the image for the workers could also be provided via
  AWS ECR (EC2 Container Registry) - or a similar service on other providers.
  This would likely improve privacy, and lessen out-of-cloud network traffic
  when the image needs to be fetched, also potentially improving container
  startup times.

- This tutorial is supposed to be adaptable to any other cloud provider: Azure,
  Google Cloud Platform, DigitalOcean, etc.

- The Docker image could be built automatically on Docker Cloud if the
  Dockerfile was hosted in a GitHub or BitBucket repository. While this is often
  useful, its advantages are questionable in this exact situation.

- Instead of RQ, Celery could also be used as job queue.

- AWS has a specialized service for job queuing, called Batch. Azure also offers
  a similar service with the same name. We could also have used AWS Batch for
  scheduling instead of running our own job queue. We chose not to use it to
  facilitate porting of the solution to other cloud providers.

- Docker Cloud can also be used to
  `deploy and manage a swarm <https://docs.docker.com/docker-cloud/cloud-swarm/>`_
  on AWS or Azure instead of their own container services.

- If the worker function itself needs to be adjusted often, the image needs to
  be rebuilt, and the containers need to be restarted each time. This can be
  avoided by synchronizing the script to the workers using the same methods as
  described above for model code distribution. The ``rq worker`` process would
  still need to be restarted when the script changed, so it is reloaded.
