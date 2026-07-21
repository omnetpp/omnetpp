Examining the Code
==================

This page describes in detail the contents of each used code file.

Utilities
---------

There are a few things that both the workers and the client need. These are
factored out into a common module, ``utils.py``. You can download the entire file
from :download:`here <code/utils.py>`.

In the first half of it, there are the imports it will use, and a ``QuietBytes``
helper class. We use this in place of its superclass, the built-in ``bytes`` type.
It only changes the string representation of the base type, to make it shorter
than the actual contents. There is a technical reason for using this: It reduces
the amount of data transferred to, and stored in, the Redis database.

.. literalinclude:: code/utils.py
   :language: python
   :end-before: def unzip_bytes

There are also two functions in it to handle ZIP archives in memory. The first
is for extracting, and the second is for compressing, with the option to exclude
some directories.

.. literalinclude:: code/utils.py
   :language: python
   :start-after: def unzip_bytes


Worker code
------------

And the code for the jobs, :download:`worker.py <code/worker.py>`:

.. literalinclude:: code/opp_worker.py
   :language: python
   :end-before: def run_simulation

With the actual job function:

.. literalinclude:: code/opp_worker.py
   :language: python
   :start-after: def run_simulation

The comments make its operation pretty straightforward.

The model needs to be cleaned, then rebuilt inside the container, because the
version of some basic system libraries might not match that of those present on
the host system, which would lead to incompatibility problems, possibly
preventing the simulation from starting.


Dockerfile
----------

We select the base image to be ``ubuntu:16.04``, then we install Python, pip, the
dependencies of |omnet++|, and wget.

.. literalinclude:: code/Dockerfile
   :language: dockerfile
   :end-before: RUN pip3 install

We upgrade pip using itself, then install RQ with it. It will also install the
Redis client module as a dependency. Then a few environment variables need to be
set, to make RQ use the right character encoding.

.. literalinclude:: code/Dockerfile
   :language: dockerfile
   :start-after: RUN apt-get update
   :end-before: COPY

Next, we copy the worker source code into the image, and set the working directory.

.. literalinclude:: code/Dockerfile
   :language: dockerfile
   :start-after: ENV LC_ALL
   :end-before: RUN wget

Downloading the |omnet++| 5.1.1 Core release archive from the official website,
extracting it, then deleting it. The referer URL has to be passed to ``wget``,
otherwise the server denies access. The ``--progress`` flag is there just to
reduce the amount of textual output, which would overly pollute the build log.

.. literalinclude:: code/Dockerfile
   :language: dockerfile
   :start-after: WORKDIR
   :end-before: ENV PATH

The ``bin`` directory added to the ``PATH`` environment variable (which would be
done by ``setenv`` normally). Finally the standard building procedure is performed
by running ``./configure`` and ``make``. Both graphical runtime environments and the
support for 3D rendering are disabled. The ``-j $(nproc)`` arguments to ``make``
enable it to use all your local CPU cores when building |omnet++| itself.

.. literalinclude:: code/Dockerfile
   :language: dockerfile
   :start-after: rm omnetpp-5.6.3-src-core.tgz
   :end-before: speed up recompiling

Installing ``ccache`` to make subsequent builds of the same model sources faster:

.. literalinclude:: code/Dockerfile
   :language: dockerfile
   :start-after: -j $(nproc)
   :end-before: ENTRYPOINT

And finally setting up the entry point to launch the rq worker, asking it to
keep the results only for one minute. This will be enough, because the client
will start downloading them right away, and it will reduce the amount of data
stored in the Redis database on average.

.. literalinclude:: code/Dockerfile
   :language: dockerfile
   :start-after: ENV PATH /usr/lib/ccache

Later, when we run containers from the image, we will be able to append
additional arguments to the entrypoint.


Client Software
----------------

The following file, :download:`client.py <code/client.py>`, implements the
command-line application for submitting jobs and getting the results.

First, the usual imports:

.. literalinclude:: code/client.py
   :language: python
   :end-before: def get_runs_from_filter

Then a helper function to resolve the run filter in a given configuration by
invoking the ``opp_run`` tool locally, in "query" mode. This is not strictly
necessary, since using a run filter is optional, but it's a nice addition.

.. literalinclude:: code/client.py
   :language: python
   :start-after: import utils
   :end-before: parser = argparse

Defining the arguments of the tool and parsing their values:

.. literalinclude:: code/client.py
   :language: python
   :start-after: return runs
   :end-before: Connecting to Redis at

Setting up the connection to the job queue, then using the helper function to
get the actual list of run numbers. Finally pack the model source into a
compressed archive (ZIP) in memory. A few directories are excluded from this
archive, because they are not needed by the workers, and are usually very large.

.. literalinclude:: code/client.py
   :language: python
   :start-after: args = parser.parse_args()
   :end-before: jobs =

Submitting a job into the queue for each run, storing the jobs in a list.
The run number is also written into the ``meta`` field of each job, but that is
necessary only so we know later which run was performed by a particular job, and
we can print it when it is done. The job function itself doesn't use this, only
its parameters.

.. literalinclude:: code/client.py
   :language: python
   :start-after: Size of sources
   :end-before: Waiting for results

And finally poll for the results of each job, downloading and unpacking the
output (the results) of completed jobs, and removing them from the list:

.. literalinclude:: code/client.py
   :language: python
   :start-after: jobs.append(j)

And exit when all jobs are completed.
