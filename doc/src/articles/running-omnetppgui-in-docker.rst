:author: Rudolf
:date: 2019-09-20

OMNeT++ GUI in Docker
=====================

Goals
-----

In another article, we have shown how to (and why) run simulations in a Docker
container. However, Docker in itself is not suited to running GUI applications,
so it is not ideal for developing or exploring simulations due to the
unavailability of the IDE and Qtenv. In this article, we explore options to
overcome this limitation.

Survey of options
-----------------

A common way of running GUI applications in Docker is X11, a network protocol
for remote graphical user interfaces. X11 requires an X11 server to be running,
to which clients can connect and display their GUIs on. On Linux systems, the
desktop is by default X11-based, so one doesn't need to install any extra
software. On Windows and macOS, one can install a 3rd party X11 server (for
example, VcXsrv on Windows and XQuartz on macOS).

We also looked into remote desktop solutions (like VNC or RDP), where the server
runs in the container, and the host connects to it as a client. Those, however,
do not provide the most seamless experience, as you see a virtual desktop, not
separate windows. Moreover, performance/quality might not be the best, even when
running locally on the same machine. From these two, RDP seems to be more
promising, as some versions support seamless mode where applications on the
remote machine - in our case, inside the container - can launch separate windows
on the host display. Sadly, the configuration of the RDP server is somewhat
complicated.

In the future, Wayland may become a viable alternative to X11 in our use case as
well. The IDE uses the GTK3 backend and Qtenv uses Qt5, and both of these
widget libraries come with perfectly usable Wayland support.

Choosing the proper Docker image
--------------------------------

If you want to use the GUI, you need a Docker image that contains the IDE,
Qtenv, and all the libraries necessary for their operation. (Many OMNeT++ Docker
images don't contain these parts, in order to keep their sizes small.)

The list of GUI-enabled OMNeT++ images is available at the following URL:
https://hub.docker.com/r/omnetpp/omnetpp-gui/tags


Docker with X11
---------------

With the following single command you can try out OMNeT++ on any system with Docker and an X11 server on it::

    docker run --rm -it -v "$(pwd):/root/models" -u "$(id -u):$(id -g)" \
      -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY omnetpp/omnetpp-gui:u18.04-5.5.1

The 2nd ``-v`` option is used here to map the X11 socket into the container.

The `x11docker project <https://github.com/mviereck/x11docker>`_ can be useful
here. It is a collection of tricks, flags, and methods to run graphical
applications in Docker (with several ways of operation, including X11 and
Wayland support, GPU passthrough, all with various levels of isolation and
performance, etc). x11docker allows the above command line to be simplified to::

    x11docker -i -- --rm -v "$(pwd):/root/models" -- omnetpp/omnetpp-gui:u18.04-5.5.1


Feedback
--------

If you found this interesting or useful, make use of the it in practice, or you
have ideas on how to take it further, please let us know!
