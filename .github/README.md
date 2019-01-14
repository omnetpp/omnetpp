[![Join the chat at https://gitter.im/omnetpp/community](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/omnetpp/community)

# OMNeT++

OMNeT++ is a public-source, component-based, modular and open-architecture
simulation environment with strong GUI support and an embeddable simulation
kernel. Its primary application area is the simulation of communication
networks, but it has been successfully used in other areas like the simulation
of IT systems, queueing networks, hardware architectures and business processes
as well.

See the main OMNeT++ website [omnetpp.org](https://omnetpp.org) for documentation,
tutorials and other introductory materials, release downloads, model catalog,
and other useful information.

## License

OMNeT++ is distributed under the [Academic Public License](../doc/License).

## Installation

To compile OMNeT++ after cloning the repository, first create your local copy
of the `configure.user` file:

    cd omnetpp
    cp configure.user.dist configure.user

then follow instructions in the [Installation
Guide](https://doc.omnetpp.org/omnetpp/InstallGuide.pdf) of the last release.
It contains OS-specific information on installing dependencies, selecting build
options, compilation, and more. (The hyperlinked PDF is generated from the
sources in `doc/src/installguide`.)
