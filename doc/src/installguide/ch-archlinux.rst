Arch Linux
==========

Supported Releases
------------------

This chapter provides additional information for installing |omnet++| on Arch Linux. The overall
installation procedure is described in the *Linux* chapter.

These instructions assume you are using the ``pacman`` package manager.

Installing the Prerequisite Packages
------------------------------------

First, ensure your system's package database is up to date and install the core development tools and libraries:

.. code::

   $ sudo pacman -Syu --needed --noconfirm make diffutils ccache clang pkgconf lld lldb gdb \
       bison gawk flex perl python python-pip uv libxml2 zlib doxygen graphviz \
       xdg-utils libdwarf

Next, install packages for the graphical environment (Qtenv and IDE). If you do not need GUI support (e.g., for a server installation), you can skip this step and later configure |omnet++| with ``WITH_QTENV=no`` and ``WITH_OSG=no``.

.. code::

   $ sudo pacman -S --needed --noconfirm qt6-base qt6-svg qt6-wayland webkit2gtk-4.1

For 3D visualization support in Qtenv, install the OpenSceneGraph package. If you do not need 3D support, you can skip this step and later configure |omnet++| with ``WITH_OSG=no``.

.. code::

   $ sudo pacman -S --needed --noconfirm openscenegraph

After installing system packages, it's good practice to clean the package cache:

.. code::

   $ sudo pacman -Scc --noconfirm

.. note::
   The commands above install Clang as the C++ compiler and LLD as the linker. If you prefer to use GCC and the system's default linker, you can adjust the package list accordingly (e.g., replace ``clang`` with ``gcc`` and omit ``lld``) and set the ``PREFER_CLANG=no`` and ``PREFER_LLD=no`` options in the ``configure.user`` file or during the ``./configure`` step.
   If you skip the GUI or 3D packages, remember to disable the corresponding features (``WITH_QTENV=no``, ``WITH_OSG=no``) in ``configure.user`` or during the ``./configure`` step.

To enable the optional parallel simulation support you will need to install an MPI package (e.g., OpenMPI):

.. code::

   $ sudo pacman -S --needed --noconfirm openmpi

Refer to the Arch Linux documentation for managing MPI environments if needed.
