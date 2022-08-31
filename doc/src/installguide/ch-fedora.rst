Fedora 33
=========

Supported Releases
------------------

This chapter provides additional information for installing |omnet++| on Fedora installations. The overall installation
procedure is described in the *Linux* chapter.

The following Fedora release is covered:

-  Fedora 33

It was tested on the following architectures:

-  Intel 64-bit

Opening a Terminal
------------------

Open the Search bar, and type *Terminal*.

Installing the Prerequisite Packages
------------------------------------

You can perform the installation using the graphical user interface or from the terminal, whichever you prefer.

Command-Line Installation
~~~~~~~~~~~~~~~~~~~~~~~~~

To install the required packages, type in the terminal:

.. code::

   $ sudo dnf install make gcc gcc-c++ clang lld bison flex perl \
       python3 python3-pip qt5-devel libxml2-devel \
       zlib-devel doxygen graphviz webkitgtk
   $ python3 -m pip install --user --upgrade numpy pandas \
       matplotlib scipy seaborn posix_ipc

To use 3D visualization support in Qtenv, you should install OpenSceneGraph 3.2 or later and osgEarth 2.7 or later
(recommended):

.. code::

   $ sudo dnf install OpenSceneGraph-devel osgearth-devel

.. note::

   You may opt to use gcc instead of the clang compiler and/or use the system default linker instead of *lld* by setting
   the ``PREFER_CLANG`` and ``PREFER_LLD`` variables in the *configure.user* file. In this case, you don’t have to
   install the ``clang`` and ``lld`` packages. If you do not need the 3D visualization capabilities, you can disable
   them in the *configure.user* file, too.

To enable the optional parallel simulation support you will need to install the MPI package:

.. code::

   $ sudo dnf install openmpi-devel

Note that *openmpi* will not be available by default, it needs to be activated in every session with the

.. code::

   $ module load mpi/openmpi-x86_64

command. When in doubt, use ``module avail`` to display the list of available modules. If you need MPI in every session,
you may add the ``module load`` command to your startup script (``.bashrc``).

Graphical Installation
~~~~~~~~~~~~~~~~~~~~~~

The graphical package manager can be launched by opening the Search bar and typing *dnf*.

Search for the following packages in the list. Select the checkboxes in front of the names, and pick the latest version
of each package.

The packages:

-  make, bison, gcc, gcc-c++, clang, lld, flex, perl, python3, python3-pip, qt5-devel, libxml2-devel, zlib-devel,
   webkitgtk, doxygen, graphviz, openmpi-devel, OpenSceneGraph-devel, osgearth-devel

Click *Apply*, then follow the instructions.

Open a terminal and enter the following command to install the required Python packages:

.. code::

   $ python3 -m pip install --user --upgrade numpy pandas matplotlib scipy \
       seaborn posix_ipc
