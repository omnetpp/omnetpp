Installation Instructions
=========================

For Linux and macOS
-------------------

To install OMNeT++, execute the `install.sh` script located in the
root directory of the OMNeT++ source distribution. This script will
automatically detect your operating system and package manager, and
proceed with the installation.

    $ ./install.sh

After the installation completes, and for any subsequent use of
OMNeT++, ensure you run the following command in your shell:

    $ source setenv

[!NOTE] Only a limited number of Linux distributions were tested
with the script (Ubuntu, Fedora). You may need to adjust the script
depending on the system package manager of your dstribution.

For Windows
-----------

Begin by executing the `mingwenv.cmd` command. In the terminal
window that opens, run the following commands:

    $ ./configure
    $ make -j16

[!NOTE] The `mingwenv.cmd` command automatically installs all
required dependencies and then executes the `source setenv` command.
You can adjust the `-j16` parameter to match the number of cores on your
machine for optimized performance.

Manual Installation
-------------------

If you encounter any issues during the installation, please
refer to the Install Guide. You might need to adjust the package
names according to your OS version.

Comprehensive installation instructions are available in the 
`doc/InstallGuide.pdf` file. This guide includes information on
dependencies, optional packages, various build options, and specific
instructions for several Linux distributions (such as recent versions
of Ubuntu and Fedora), macOS, and Windows. We highly recommend 
reviewing this guide before installing OMNeT++.

If you find any errors or omissions in the Install Guide, please let
us know so we can improve it.

For experienced users who have all dependencies already installed,
you can directly enter the following commands (on Linux and macOS,
using bash or zsh):

    $ source setenv 
    $ ./configure 
    $ make -j16

We hope you enjoy working with OMNeT++!