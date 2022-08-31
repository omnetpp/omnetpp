Red Hat
=======

Supported Releases
------------------

This chapter provides additional information for installing |omnet++| on Red Hat Enterprise Linux installations. The
overall installation procedure is described in the *Linux* chapter.

The following Red Hat release is covered:

-  Red Hat Enterprise Linux Desktop Workstation 8.x

It was tested on the following architectures:

-  Intel 64-bit

Opening a Terminal
------------------

Choose *Applications > Accessories > Terminal* from the menu.

Installing the Prerequisite Packages
------------------------------------

You can perform the installation using the graphical user interface or from the terminal, whichever you prefer.

.. note::

   You will need Red Hat Enterprise Linux Desktop Workstation for |omnet++|. The *Desktop Client* version does not
   contain development tools.

Command-Line Installation
~~~~~~~~~~~~~~~~~~~~~~~~~

To install the required packages, type in the terminal:

.. code::

   $ su -c 'yum install make gcc gcc-c++ clang lld bison flex perl \
       python3 python3-pip qt-devel libxml2-devel zlib-devel doxygen \
       graphviz xdg-utils'
   $ python3 -m pip install --user --upgrade numpy pandas matplotlib scipy \
       seaborn posix_ipc

To use 3D visualization support in Qtenv (recommended), you should install the OpenSceneGraph-devel (3.2 or later) and
osgEarth-devel (2.7 or later) packages. These packages are not available from the official RedHat repository so you may
need to get them from different sources (e.g. rpmfind.net).

.. warning::
   
   The IDE requires GLIBC 2.28 version or later, so RedHat 6 and 7 is NOT supported.

.. note::

   You may opt to use gcc instead of the clang compiler and/or use the system default linker instead of *lld* by setting
   the ``PREFER_CLANG`` and ``PREFER_LLD`` variables in the *configure.user* file. In this case, you don’t have to
   install the ``clang`` and ``lld`` packages. If you do not need the 3D visualization capabilities, you can disable
   them in the *configure.user* file, too.

To install additional (optional) packages for parallel simulation, type:

.. code::

   $ su -c 'yum install openmpi-devel'

Note that *openmpi* will not be available by default, it needs to be activated in every session with the

.. code::

   $ module load openmpi_<arch>

command, where ``<arch>`` is your architecture (usually ``i386`` or ``x86_64``). When in doubt, use ``module avail`` to
display the list of available modules. If you need MPI in every session, you may add the ``module load`` command to your
startup script (``.bashrc``).\`

Graphical Installation
~~~~~~~~~~~~~~~~~~~~~~

The graphical installer can be launched by choosing *Applications > Add/Remove Software* from the menu.

Search for the following packages in the list. Select the checkboxes in front of the names, and pick the latest version
of each package.

The packages:

-  make, gcc, gcc-c++, clang, lld, bison, flex, perl, python3, qt-devel, libxml2-devel, zlib-devel, doxygen, graphviz,
   openmpi-devel, xdg-utils

Click *Apply*, then follow the instructions.

SELinux
-------

You may need to turn off SELinux when running certain simulations. To do so, click on *System > Administration >
Security Level > Firewall*, go to the *SELinux* tab, and choose *Disabled*.

You can verify the SELinux status by typing the ``sestatus`` command in a terminal.

.. note::

   From |omnet++| 4.1 on, makefiles that build shared libraries include the ``chcon -t textrel_shlib_t lib<name>.so``
   command that properly sets the security context for the library. This should prevent the SELinux-related *"cannot
   restore segment prot after reloc: Permission denied"* error from occurring, unless you have a shared library which
   was built using an obsolete or hand-crafted makefile that does not contain the ``chcon`` command.
