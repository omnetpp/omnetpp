:orphan:

Windows - Using WSL
===================

Windows Subsystem for Linux (WSL) supports running a full Linux distribution on a Windows machine.
Running |omnet++| in WSL 2 has several advantages compared to running |omnet++| natively on Windows:

Advantages:

-  You will probably see **significant** speedup on certain tasks (like compilation) compared to the native Windows
   (MinGW64) toolchain, because the compiler toolchain and the filesystem (ext4) is much faster in WSL 2 than their
   Windows equivalents.

-  The native MinGW64 toolchain on Windows is basically a mini (Unix-like) system, emulated on top of Windows. Because
   of the emulation, it may have incompatibilities and limitations compared to the Linux tools. You will have fewer
   issues and surprises when running |omnet++| on Linux.

Disadvantages:

-  You will not be able to link against Windows libraries, however this is seldom needed as almost all libraries are
   available in the Linux environment, too.


Enabling or Upgrading WSL 2 on Windows
--------------------------------------

Installing |omnet++| on WSL is supported on WSL 2.5.7 or later.

Open a PowerShell with Administrator privileges. On newer versions of Windows, you can install the WSL subsystem by
typing:

.. code::

   wsl --install

Or if you have WSL already installed, just upgrade it to the latest version:

.. code::

   wsl.exe --upgrade

Make sure that it is 2.5.7 or later and continue to install either a Linux distribution from the
Microsoft Store or ``opp_env`` in WSL.

.. tip::

   We recommend installing and using the Windows Terminal application, which is available at
   https://www.microsoft.com/store/productId/9N0DX20HK701


Installing with opp_env.wsl
---------------------------

**opp_env.wsl** is a pre-configured Linux environment that can be easily installed on Windows and contains
the ``opp_env`` package manager, maintained by the |omnet++| team. Its main advantage is that it
can automate the installation of |omnet++| and its dependencies. Additionally, it can install a growing
list of simulation models and tools with a single, very simple command.

Just download the ``opp_env.wsl`` file from https://github.com/omnetpp/opp_env/releases/download/wsl/opp_env.wsl
and start it from your browser or the File Explorer. Then, follow the on-screen instructions to install
|omnet++| and its dependencies.

From command line you can use:

.. code::

   curl.exe -L https://github.com/omnetpp/opp_env/releases/download/wsl/opp_env.wsl | wsl --import opp_env -

For more information, visit: https://github.com/omnetpp/opp_env.


Installing a Linux distribution in WSL
--------------------------------------

As a next step, you must install a Linux distribution from the Microsoft Store. We recommend using Ubuntu from
https://apps.microsoft.com/detail/9pdxgncfsczv.

Once the installation is done, run the distro and finish the setup process by setting up a user name and password. At
this point, you could install |omnet++|.

Install |omnet++| Linux
-----------------------

At this point, you have a fully functional Linux environment that can run GUI apps. You can go on and follow the Ubuntu
specific installation steps to finally install |omnet++| on your system.


Windows - Using the MinGW64 Compiler Toolchain
==============================================

Supported Windows Versions
--------------------------

|omnet++| is supported on 64-bit versions of Windows 11.

Installing |omnet++|
--------------------

Download the |omnet++| source code from https://omnetpp.org. Make sure you select the Windows-specific archive, named
``|omnetpp|-|version|-windows-x86_64.7z``.

The package is self-contained: in addition to |omnet++| files it includes a C++ compiler, a command-line build
environment, and all libraries and programs required by |omnet++|.

Copy the |omnet++| archive to the directory where you want to install it. Choose a directory whose full path **does not
contain any space**; for example, do not put |omnet++| under *Program Files*.

Extract the archive file. To do so, right-click the file in Windows Explorer, and select *Extract All* from the menu.

When you look into the new ``|omnetpp|-|version|`` directory, should see directories named ``doc``, ``images``,
``include``, ``tools``, etc., and files named ``opp_shell.cmd``, ``configure``, ``Makefile``, and others.

Configuring and Building |omnet++|
----------------------------------

Start ``opp_shell.cmd`` in the ``|omnetpp|-|version|`` directory by double-clicking it in Windows Explorer.
It will bring up a console with the MSYS *bash* shell, where the path is already set to include the
``|omnetpp|-|version|/bin`` directory. On the first start of the shell, you may need to wait for the extraction
of the ``tools`` directory.

First, check the contents of the ``configure.user`` file to make sure it contains the settings you need. In most cases
you don't need to change anything.

.. code::

   notepad configure.user

Then enter the following commands:

.. code::

   $ ./configure
   $ make -j16

The build process will create both debug and release binaries.

.. note::

   If you want to install the dependencies manually instead of using the pre-packaged tools archive, delete all ``*.7z``
   files from the ``tools`` directory **before** starting ``opp_shell.cmd`` the first time. This will prevent the
   extraction of the pre-packaged tools. After starting ``opp_shell.cmd``, you **must** install the dependencies
   manually by executing the ``./install.sh`` script. The script will install all the dependencies and configure, then 
   build |omnet++|.


Verifying the Installation
--------------------------

You should now test all samples and check they run correctly. As an example, the *aloha* example is started by entering
the following commands:

.. code::

   $ cd samples/aloha
   $ ./aloha

By default, the samples will run using the graphical Qtenv environment. You should see GUI windows and dialogs.

Starting the IDE
----------------

|omnet++| comes with an Eclipse-based Simulation IDE. You should be able to start the IDE by typing:

.. code::

   $ |omnetpp|

We recommend that you start the IDE from the command-line. The build process will also create a shortcut for you
if you want to use the start menu.

.. warning:: 

   Pinning the |omnet++| IDE to the taskbar will **NOT** work.

Environment Variables
---------------------

In general |omnet++| requires that certain environment variables are set. Always use the 
the provided shell window to start the IDE or your simulations.

Reconfiguring the Libraries
---------------------------

If you need to recompile the |omnet++| components with different flags (e.g. different optimization), then change the
top-level |omnet++| directory, edit ``configure.user`` accordingly, then type:

.. code::

   $ ./configure
   $ make clean
   $ make -j16

If you want to recompile just a single library, then change to the directory of the library (e.g. ``cd src/sim``) and
type:

.. code::

   $ make clean
   $ make

By default, libraries are compiled in both debug and release mode. If you want to make release or debug builds only,
use:

.. code::

   $ make MODE=release

or

.. code::

   $ make MODE=debug

By default, shared libraries will be created. If you want to build static libraries, set ``SHARED_LIBS=no`` in
``configure.user`` and re-configure your project.

.. note::

   The built libraries and programs are immediately copied to the ``lib/`` and ``bin/`` subdirs.

Portability Issues
------------------

|omnet++| has been tested with both the clang compiler from the MinGW-w64 package.

Microsoft Visual C++ is not supported in the Academic Edition.

Additional Packages
-------------------

MPI
~~~

MPI is only needed if you would like to run parallel simulations.

There are several MPI implementations for Windows, and |omnet++| does not mandate any specific one. We recommend
DeinoMPI, which can be downloaded from http://mpi.deino.net.

After installing DeinoMPI, adjust the ``MPI_DIR`` setting in |omnet++|'s ``configure.user``, and reconfigure and
recompile |omnet++|:

.. code::

   $ ./configure
   $ make cleanall
   $ make

.. note::

   In general, if you would like to run parallel simulations, we recommend that you use Linux, macOS, or another
   unix-like platform.

Akaroa
~~~~~~

Akaroa 2.7.9, which is the latest version at the time of writing, does not support Windows. You may try to port it using
the porting guide from the Akaroa distribution.
