General Information
===================

Introduction
------------

This document describes how to install |omnet++| on various platforms. One chapter is dedicated to each operating
system.

Supported Platforms
-------------------

|omnet++| has been tested and is supported on the following operating systems:

-  Windows on x86_64 architecture
-  MacOS 10.15 and 11.x on x86_64 architecture
-  Linux distributions covered in this Installation Guide

.. ifconfig:: what=='omnest'

   64-bit precompiled binaries are provided for the following platforms:

   -  Windows with Microsoft Visual C++ 2017 / ClangC2
   -  Windows with the bundled MinGW-w64 gcc/clang compiler

   On other platforms, |omnet++| needs to be compiled from source.

The Simulation IDE is supported on the following platforms:

-  Linux x86_64/aarch64
-  Windows x86_64
-  MacOS 10.15 and 11.x (x86_64)

.. note::

   Simulations can be run practically on any unix-like environment with a decent and fairly up-to-date C++ compiler,
   for example gcc 8.x. Certain |omnet++| features (Qtenv, parallel simulation, XML support, etc.) depend on the
   availability of external libraries (Qt, MPI, LibXML, etc.)

   IDE platforms are restricted because the IDE relies on a native shared library, which we compile for the above
   platforms and distribute in binary form for convenience.
