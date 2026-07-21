:author: Rudolf
:date: 2019-03-20

Building an OMNeT++ Distro
==========================

Recently we started to use Docker to build OMNeT++ releases. This approach gave us reproducible results and independence
from the local development environment. While previously we needed a carefully configured Jenkins build server, from now
on anyone can re-create the release archives by simply checking out the OMNeT++ sources, going into the ``releng``
directory and executing the ``build-omnetpp-in-docker`` script (as long as ``git`` and ``docker`` are already installed
on the system). The docker images required by the build process are available on DockerHub.

Go and give it a try if you need to build an OMNeT++ distro for whatever reason.
