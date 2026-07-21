:author: Rudolf
:date: 2020-09-17

Installing OMNeT++ on Windows Subsystem for Linux (WSL 2)
=========================================================

Motivation
----------

Windows Subsystem for Linux version 2 (WSL 2) is a Windows facility for running
a full-blown Linux distribution in Windows. Running OMNeT++ in WSL 2 has certain
advantages compared to running OMNeT++ natively on Windows:

Advantages:

* You will probably see significant speedup on certain tasks (like compilation) compared to
  the native Windows (MinGW64) toolchain, because the compiler toolchain
  and the filesystem (ext4) is much faster in WSL 2 than their Windows equivalents.

* The native MinGW64 toolchain on Windows is basically a mini (Unix-like) system,
  emulated on top of Windows. Because of the emulation, it may have incompatibilities and
  limitations compared to the Linux tools. You will have fewer issues and surprises when running
  OMNeT++ on Linux.

There are a few drawbacks as well:

* WSL 2 does not (yet) support running Linux GUI applications.
  This means that you must install and run an X Server process on Windows to be able
  to use any GUI tools (i.e. IDE, Qtenv, etc.) from OMNeT++.

* Because of a limitation of the available X Server software, 3D acceleration is not working.
  You will not be able to use the OMNeT++ OpenSceneGraph and osgEarth integration in this setup and it is
  recommended to explicitly disable these features when you build OMNeT++.

Supported Windows Versions
--------------------------

Installing OMNeT++ on WSL 2 is supported on Windows 10 version 1903
(build 18362.1049) or later. Note especially the minor build number.
Your Windows version must have at least 1049 as a minor build number.

Enabling WSL 2 on Windows
-------------------------

Open a PowerShell with Administrator privileges. On newer versions of Windows,
you can install the WSL subsystem by typing::

    wsl --install

Alternatively, if your system does not have a ``wsl`` command, use the following commands::

    dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
    dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart

After a successful installation, reboot your computer and open an Administrator
PowerShell again to set the default WSL version to 2::

    wsl.exe --set-default-version 2

.. tip::
    We recommend installing and using the Windows Terminal application,
    which is available at https://www.microsoft.com/store/productId/9N0DX20HK701

Installing an Ubuntu distribution
---------------------------------

As a next step, you must install a Linux distribution from the Microsoft Store.
We recommend using Ubuntu 20.04 from https://www.microsoft.com/store/productId/9n6svws3rx71.

Once the installation is done, run the distro and finish the setup process by setting up a
user name and password. At this point, you could install OMNeT++, but GUI programs
would not work.

Install VcXserver
-----------------

To use GUI programs from Linux, you must install an X Server application from:
https://sourceforge.net/projects/vcxsrv/

Start the installation and make sure that you:

- select "Disable access control"
- set display number to 0
- check "Private networks, such as my home or work network" and  click "Allow access"
  when the Windows Defender Firewall asks for permission.

Open the Windows Terminal and launch the Ubuntu distribution from the dropdown menu.
Add the following line to the ``/etc/bash.bashrc`` or ``~/.bashrc`` file::

    export DISPLAY=$(grep -m 1 nameserver /etc/resolv.conf | awk '{print $2}'):0.0

This will ensure that Linux programs will always find the X Server process running on Windows.
Exit from the Ubuntu shell, and restart it to make sure that the change was applied correctly.
Check if::

    $ echo $DISPLAY

displays the correct IP address of the Windows machine.

In the future, make sure that the X Server is always running when you want to run
Linux GUI programs by either making the X Server automatically start or launching it manually.

.. tip::
    There is ongoing work to make Linux GUI applications work
    on Windows by default. On later versions of Windows you may be
    able to skip the whole X Server installation step.

Install the Ubuntu Version of OMNeT++ 
-------------------------------------

At this point, you have a fully functional Linux environment that can run GUI apps.
You can go on and follow the Ubuntu specific installation steps to finally install OMNeT++
on your system.

Usage
-----

Make sure the X server is running. Then, open the Windows Terminal and launch
the Ubuntu distribution from the dropdown menu. From this point on, you can
use OMNeT++ exactly as you would use it on Ubuntu.
