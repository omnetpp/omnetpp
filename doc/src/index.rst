
.. toctree::
   :caption: Manual
   :maxdepth: 2
   :numbered: 3
   :titlesonly:
   :hidden:

   Simulation Manual <manual/index>

.. toctree::
   :caption: API Reference
   :maxdepth: 2
   :hidden:

   Simulation Library API <api/api-index>
   nedxml-api/nedxml-api-index
   parsim-api/parsim-api-index
   Python API <python-api-index>

.. toctree::
   :maxdepth: 2
   :caption: Installation
   :hidden:

   installguide/index

.. toctree::
   :maxdepth: 2
   :caption: Using the IDE
   :hidden:

   ide-overview/ide-overview
   IDE Users Guide <userguide/index>
   ide-developersguide/ide-developersguide
   ide-customization-guide/ide-customization-guide


.. toctree::
   :maxdepth: 2
   :caption: Tutorials
   :hidden:

   tutorials/tictoc/index
   tutorials/cloud/index
   tutorials/swarm/index

.. toctree::
   :maxdepth: 2
   :caption: Articles
   :glob:
   :hidden:

   articles/*

Overview of OMNeT++
===================

.. abstract:: What is OMNeT++?

   OMNeT++ is an extensible, modular, component-based C++ simulation
   library and framework, primarily for building network simulators.
   “Network” is meant in a broader sense that includes wired and
   wireless communication networks, on-chip networks, queueing networks,
   and so on. Domain-specific functionality such as support for sensor
   networks, wireless ad-hoc networks, Internet protocols, performance
   modeling, photonic networks, etc., is provided by model frameworks,
   developed as independent projects. OMNeT++ offers an Eclipse-based
   IDE, a graphical runtime environment, and a host of other tools.
   There are extensions for real-time simulation, network emulation,
   database integration, SystemC integration, and several other
   functions. OMNeT++ is distributed under the `Academic Public
   License <license>`__.

Although OMNeT++ is not a network simulator itself, it has gained
widespread popularity as a network simulation platform in the scientific
community as well as in industrial settings, and building up a large
user community.

OMNeT++ provides a component architecture for models. Components
*(modules)* are programmed in C++, then assembled into larger components
and models using a high-level language *(NED)*. Reusability of models
comes for free. OMNeT++ has extensive GUI support, and due to its
modular architecture, the simulation kernel (and models) can be embedded
easily into your applications.

Components
----------

The main ingredients of OMNeT++ are:

- Simulation kernel library (C++)
- The NED topology description language
- Simulation IDE based on the Eclipse platform
- Interactive simulation runtime GUI (Qtenv)
- Command-line interface for simulation execution (Cmdenv)
- Utilities (makefile creation tool, etc.)
- Documentation, sample simulations, etc.

Models
------

During the years OMNeT++ has been available, countless simulation models
and model frameworks have been written for it by researchers in diverse
areas: queuing, resource modeling, internet protocols, wireless
networks, switched LANs, peer-to-peer networks, media streaming, mobile
ad-hoc networks, mesh networks, wireless sensor networks, vehicular
networks, NoCs, optical networks, HPC systems, cloud computing, SANs,
and more. Most of these model frameworks are open source, developed as
independent projects, and follow their own release cycles.

The `INET Framework <http://inet.omnetpp.org>`__ can be considered the
standard protocol model library of OMNeT++. INET contains models for the
Internet stack and many other protocols and components. The INET
Framework is maintained by the OMNeT++ team for the community, utilizing
patches and new models contributed by members of the community. Several
other simulation frameworks take INET as a base, and extend it into
specific directions, such as vehicular networks (Veins, CoRE),
overlay/peer-to-peer networks (OverSim), or LTE (SimuLTE).

We maintain a list of selected OMNeT++ models and model frameworks
`here <https://omnetpp.org/download/models-and-tools>`__.

Workflow
--------

Here is a a brief overview of the workflow for using OMNeT++:

1. An OMNeT++ model is built from components (modules) which communicate
   by exchanging messages. Modules can be nested, that is, several
   modules can be grouped together to form a compound module. When
   creating the model, you need to map your system into a hierarchy of
   communicating modules. (When you use a model framework, this and the
   next two steps are mostly done for you.)

2. Define the model structure in the NED language. You can edit NED in a
   text editor or in the graphical editor of the Eclipse-based OMNeT++
   Simulation IDE.

3. The active components of the model (simple modules) are programmed in
   C++, using the simulation kernel and class library. C++ classes that
   represent protocol headers are described in MSG files which are then
   translated into C++ code.

4. Provide a suitable ``omnetpp.ini`` file to hold OMNeT++ configuration
   and parameters to your model. One ini file may hold several
   configurations which can build on one another, and may even contain
   parameter studies.

5. Build the simulation program and run it. You’ll link the code with
   the OMNeT++ simulation kernel and one of the user interfaces OMNeT++
   provides. There are command line and interactive, graphical user
   interfaces.

6. Simulation results are written into output vector and output scalar
   files. You can use the Analysis Tool powered by Pandas and Matplotlib
   in the Simulation IDE to analyze and plot them. Event logs recorded
   from the simulation can be viewed in the Sequence Chart Tool in the
   IDE. Result files are text-based, so you can also process them with
   R, Matlab or other tools.

Simulation IDE
--------------

The Simulation IDE is a powerful, feature-rich environment for
developing, running, and evaluating simulation models. Please note that
using the IDE is entirely optional; model development and execution are
also possible and fully supported through the command line or
alternative editors/IDEs.

The following video provides a brief introduction to the IDE.

.. youtube:: KUe_vrWbEJY
   :width: 560
   :height: 315
   :align: center

Platforms
---------

The OMNeT++ simulation kernel is standard C++, and runs basically on all
platforms where a modern C++ compiler is available. The Simulation IDE
requires Windows, Linux, or macOS.
