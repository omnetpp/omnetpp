Quick Installation
==================

Introduction
------------

This document describes how to install |omnet++| on various platforms. One chapter is dedicated to each operating
system.

Supported Platforms
-------------------

|omnet++| (including the Simulation IDE) has been tested and is supported
on the following operating systems:

-  Linux x86_64/aarch64 distributions covered in this Installation Guide
-  macOS 15 or later on x86_64/aarch64 architectures
-  Windows 11 on x86_64 architecture

.. note::

   Simulations can be run practically on any unix-like environment with a decent and fairly up-to-date C++ compiler,
   for example gcc 14.x. Certain |omnet++| features (Qtenv, parallel simulation, XML support, etc.) depend on the
   availability of external libraries (Qt, MPI, LibXML, etc.)

   IDE platforms are restricted because the IDE relies on a native shared library, which we compile for the above
   platforms and distribute in binary form for convenience.

Recommended Installation Method (opp_env)
-----------------------------------------

The recommended installation method is to use ``opp_env install omnetpp-latest``.
``opp_env`` is a package manager for |omnet++| and its dependencies. You can
download it from https://github.com/omnetpp/opp_env. Its main advantage is that it
can automate the installation of any version of |omnet++|, its dependencies and also
various simulation models and tools.

``opp_env`` is supported only on Linux and macOS. To install it on Windows, use the
WSL (Windows Subsystem for Linux) feature of Windows 11. See further details in the
chapter :ref:`ch-windows-omnetpp`.

Installing |omnet++| with the System Package Manager
----------------------------------------------------

To install |omnet++| using the system package manager, start the 
``install.sh`` script in the root directory of the |omnet++| installation.
The script will detect the system package manager and will guide you through
the installation process.

Manual Installation
-------------------

To manually install |omnet++| and its dependencies, visit the appropriate chapter
for your platform.
