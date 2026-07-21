:author: Rudolf
:date: 2019-06-20

Using Docker for Running Simulations on Various Versions of OMNeT++
===================================================================

Goals
-----

It is often challenging to get models written for older versions of OMNeT++
working in contemporary Linux distributions. Older versions of OMNeT++ are often
not compatible with newer Linux distibutions: changes in the C++ compiler and in
the dependencies (library versions, etc.) often cause installation failure.

One solution to this problem is using Docker. We provide Docker images that
contain older versions OMNeT++, already in compiled form. It is possible
to compile and run the simulation models in these Docker images.

How to use the OMNeT++ Docker images
------------------------------------

As images are published on the Docker Hub, it is straightforward to deploy
OMNeT++ on any machine that has Docker.

You can see the list of the available images here: https://hub.docker.com/r/omnetpp/omnetpp/tags.
Image tags have the syntax ``u18.04-5.5.1``, where the first part is the Ubuntu version,
and the second part is the OMNeT++.

After choosing the suitable image, change into the directory of the simulation
model, and issue the following command (replace the end of the last argument
with the proper image tag)::

    docker run --rm -it -v "$(pwd):/root/models" -u "$(id -u):$(id -g)" omnetpp/omnetpp:u18.04-5.5.1

This command will download the image (unless already downloaded), and opens a
shell inside the container. The current working directory will be mapped to
``/root/models`` inside the Docker container. Then follow the build instructions
of the simulation model (for example, type ``make``). When the build process
completes, you can run the simulation.

Note that you can only run simulation under Cmdenv and in release mode, and
cannot use the IDE. The reason is that we wanted to keep the size of these
Docker images relatively small. Including the IDE, debug mode libraries, the Qt
libraries for Qtenv, etc. would have blown up the image size considerably.

Everything that is created under ``/root/models`` inside the Docker image, such as
build artifacts and simulation result files, will be available in your local
file system (in the directory where you issued the Docker command).

This setup lets edit the model files outside of the container with your favorite editor.

Further possible uses
---------------------

We are internally using these Docker images for continuous testing of INET.

A future possible use case is creating and publishing reproducible simulations.
The researcher who publishes the simulation would create a Docker image based on
one of the OMNeT++ images, and push it to Docker Hub. The new image would
contain a runnable version of the simulation. The label of the new image would
be advertised, for example inside the corresponding research paper. Any
researcher interested in the study would be able to pull and run the image to
reproduce the results.

Alternatively, it could also be sufficient to publish the hash of the commit
containing the "final" version of the model in a public Git repository, plus
the tag of the OMNeT++ Docker image required for running it. This would also
allow 3rd party researchers to build and run the model in a reproducible way.
