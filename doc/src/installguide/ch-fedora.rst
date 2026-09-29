Fedora
======

Supported Releases
------------------

This chapter provides additional information for installing |omnet++| on Fedora installations. The overall installation
procedure is described in the *Linux* chapter.

The following Fedora release is known to work:

-  Fedora 42 (and likely newer versions)

Installing the Prerequisite Packages
------------------------------------

To install the required packages, type in the terminal.

First, install the core development tools and libraries:

.. code::

   $ sudo dnf install -y make ccache clang awk lld lldb gdb bison flex perl \
       python3-devel python3-pip uv libxml2-devel zlib-devel doxygen graphviz \
       xdg-utils libdwarf-devel

Next, install packages for the graphical environment (Qtenv and IDE). If you do not need GUI support (e.g., for a server installation), you can skip this step and later configure |omnet++| with ``WITH_QTENV=no`` and ``WITH_OSG=no``.

.. code::

   $ sudo dnf install -y qt6-qttools-devel qt6-qtbase-devel qt6-qtsvg \
       qt6-qtwayland webkit2gtk4.1

For 3D visualization support in Qtenv, install the OpenSceneGraph development package. If you do not need 3D support, you can skip this step and later configure |omnet++| with ``WITH_OSG=no``.

.. code::

   $ sudo dnf install -y OpenSceneGraph-devel

After installing system packages, it's good practice to clean the local repository of retrieved package files:

.. code::

   $ sudo dnf clean packages

Next, set up a Python virtual environment for |omnet++| using ``uv``. In the root directory of your |omnet++| download:

.. code::

   $ uv venv --python python3 --seed --prompt "omnetpp/.venv" .venv
   $ source .venv/bin/activate

Then, install the required Python packages into the virtual environment:

.. code::

   $ uv pip install -r python/requirements.txt

.. note::

   If you prefer not to use ``uv``, create the virtual environment with
   ``python3 -m venv .venv --upgrade-deps --clear --prompt "omnetpp/.venv"`` and install the packages with
   ``python3 -m pip install -r python/requirements.txt`` instead.

.. note::

   The commands above install Clang as the C++ compiler and LLD as the linker. If you prefer to use GCC and the system's default linker, you can adjust the package list accordingly (e.g., replace ``clang`` with ``g++`` and omit ``lld``) and set the ``PREFER_CLANG=no`` and ``PREFER_LLD=no`` options in the ``configure.user`` file or during the ``./configure`` step.
   If you skip the GUI or 3D packages, remember to disable the corresponding features (``WITH_QTENV=no``, ``WITH_OSG=no``) in ``configure.user`` or during the ``./configure`` step.

To enable the optional parallel simulation support you will need to install the MPI package:

.. code::

   $ sudo dnf install openmpi-devel

Note that *openmpi* will not be available by default, it needs to be activated in every session with the

.. code::

   $ module load mpi/openmpi-x86_64

command. When in doubt, use ``module avail`` to display the list of available modules. If you need MPI in every session,
you may add the ``module load`` command to your startup script (``.bashrc``).
