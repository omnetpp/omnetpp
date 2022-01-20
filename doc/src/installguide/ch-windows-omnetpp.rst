:orphan:

Windows - Using the MinGW64 Compiler Toolchain
==============================================

Supported Windows Versions
--------------------------

|omnet++| is supported only on 64-bit versions of Windows.

.. note::

   32-bit Windows versions are no longer supported. If you need 32-bit builds on Windows, we recommend using |omnet++|
   5.0

Installing |omnet++|
--------------------

Download the |omnet++| source code from https://omnetpp.org. Make sure you select the Windows-specific archive, named
``|omnetpp|-|version|-windows-x86_64.zip``.

The package is self-contained: in addition to |omnet++| files it includes a C++ compiler, a command-line build
environment, and all libraries and programs required by |omnet++|.

Copy the |omnet++| archive to the directory where you want to install it. Choose a directory whose full path **does not
contain any space**; for example, do not put |omnet++| under *Program Files*.

Extract the zip file. To do so, right-click the zip file in Windows Explorer, and select *Extract All* from the menu.
You can also use external programs like Winzip or 7zip.

When you look into the new ``|omnetpp|-|version|`` directory, should see directories named ``doc``, ``images``,
``include``, ``tools``, etc., and files named ``mingwenv.cmd``, ``configure``, ``Makefile``, and others.

Configuring and Building |omnet++|
----------------------------------

Start ``mingwenv.cmd`` in the ``|omnetpp|-|version|`` directory by double-clicking it in Windows Explorer. It will bring
up a console with the MSYS *bash* shell, where the path is already set to include the ``|omnetpp|-|version|/bin``
directory. On the first start of the shell, you may need to wait for the extraction of the ``tools`` directory.

.. note::

   If you want to start simulations from outside the shell as well (for example from Explorer), you need to add
   |omnet++|'s ``bin`` directory and also the ``bin`` directories in the tools folder to the path; instructions are
   provided later.

First, check the contents of the ``configure.user`` file to make sure it contains the settings you need. In most cases
you don’t need to change anything.

.. code::

   notepad configure.user

Then enter the following commands:

.. code::

   $ ./configure
   $ make

The build process will create both debug and release binaries.

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

We recommend that you start the IDE from the command-line. You can also create a shortcut for starting the IDE. To do so,
locate the ``opp_ide.exe`` program in the ``|omnetpp|-|version|/ide`` directory in Windows Explorer, right-click it, and
choose *Send To > Desktop (create shortcut)* from the menu. You can right-click the taskbar icon while the IDE is
running, and select *Pin this program to taskbar* from the context menu.

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
   $ make

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

|omnet++| has been tested with both the gcc and the clang compiler from the MinGW-w64 package.

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

Windows - Using Windows Subsystem for Linux (WSL) version 2
===========================================================

WSL 2 supports running a full Linux distribution on a Windows machine. Running |omnet++| in WSL 2 has certain advantages
and disadvantages compared to running |omnet++| natively on Windows:

Advantages:

-  You will probably see significant speedup on certain tasks (like compilation) compared to the native Windows
   (MinGW64) toolchain, because the compiler toolchain and the filesystem (ext4) is much faster in WSL 2 than their
   Windows equivalents.

-  The native MinGW64 toolchain on Windows is basically a mini (Unix-like) system, emulated on top of Windows. Because
   of the emulation, it may have incompatibilities and limitations compared to the Linux tools. You will have fewer
   issues and surprises when running |omnet++| on Linux.

Disadvantages:

-  WSL 2 does not (yet) support running Linux GUI applications. This means that you must install and run an X Server
   process on Windows to be able to use any GUI tools (i.e. IDE, Qtenv, etc.) from |omnet++|.

-  Because of a limitation of the available X Server software, 3D acceleration is not working. You will not be able to
   use the |omnet++| OpenSceneGraph and osgEarth integration in this setup and it is recommended to explicitly disable
   these features when you build |omnet++|.


WSL 2 Requirements
------------------

Installing |omnet++| on WSL 2 is supported on Windows 10 version 1903 (build 18362.1049) or later. Note especially the
minor build number. Your Windows version must have at least 1049 as a minor build number.

Enabling WSL 2 on Windows
-------------------------

Open a PowerShell with Administrator privileges. On newer versions of Windows, you can install the WSL subsystem by
typing:

.. code::

   wsl --install

Alternatively, if your system does not have a ``wsl`` command, use the following commands:

.. code::

   dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
   dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart

After a successful installation, reboot your computer and open an Administrator PowerShell again to set the default WSL
version to 2.

.. code::

   wsl.exe --set-default-version 2

.. tip::

   We recommend installing and using the Windows Terminal application, which is available at
   https://www.microsoft.com/store/productId/9N0DX20HK701

Installing an Ubuntu distribution
---------------------------------

As a next step, you must install a Linux distribution from the Microsoft Store. We recommend using Ubuntu 20.04 from
https://www.microsoft.com/store/productId/9n6svws3rx71.

Once the installation is done, run the distro and finish the setup process by setting up a user name and password. At
this point, you could install |omnet++|, but GUI programs would not work.

Install VcXserver
-----------------

.. note::

   There is ongoing work (called WSLg https://github.com/microsoft/wslg) to make Linux GUI applications work on 
   Windows by default. On later versions of Windows (21H2+) you may be able to skip the whole X Server installation step.

To use GUI programs from Linux, you must install an X Server application from: https://sourceforge.net/projects/vcxsrv/

Start the installation and make sure that you:

-  select "Disable access control"
-  set display number to 0
-  check "Private networks, such as my home or work network" and click "Allow access" when the Windows Defender Firewall
   asks for permission.

Open the Windows Terminal and launch the Ubuntu distribution from the dropdown menu. Add the following line to the
``/etc/bash.bashrc`` or ``~/.bashrc`` file.

.. code::

   export DISPLAY=$(grep -m 1 nameserver /etc/resolv.conf | awk '{print $2}'):0.0

This will ensure that Linux programs will always find the X Server process running on Windows. Exit from the Ubuntu
shell, and restart it to make sure that the change was applied correctly. Check if

.. code::

   $ echo $DISPLAY

displays the correct IP address of the Windows machine.

In the future, make sure that the X Server is always running when you want to run Linux GUI programs by either making
the X Server automatically start or launching it manually.

Install |omnet++| Linux
-----------------------

At this point, you have a fully functional Linux environment that can run GUI apps. You can go on and follow the Ubuntu
specific installation steps to finally install |omnet++| on your system.
