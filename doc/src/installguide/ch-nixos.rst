NixOS
=====

Supported Releases
------------------

This chapter provides additional information for installing |omnet++| on NixOS. The overall
installation procedure is described in the *Linux* chapter.

The following NixOS releases are known to work:

-  NixOS 25.11 or later

Installation Prerequisites
--------------------------

On NixOS, the dependencies come from the Nix environment that |omnetpp| provides, so
``install.sh`` installs no system packages; it only sets up the Python virtual environment. |omnetpp| assumes that certain features are enabled in your
NixOS configuration. Make sure that ``nix.settings.experimental-features = [ "nix-command" "flakes" ];``
is present in your NixOS configuration (``/etc/nixos/configuration.nix``).

Start the ``./install.sh`` script in the root directory of the |omnetpp| installation
and follow the instructions:

.. code::

   $ ./install.sh

After the script has finished, you can start an |omnetpp| session any time by typing:

.. code::

   # set up in the current shell
   $ source setenv

   # or within a new shell
   $ ./setenv bash

Post-Installation Steps
~~~~~~~~~~~~~~~~~~~~~~~

Setting Up Debugging
^^^^^^^^^^^^^^^^^^^^

By default, NixOS does not allow ptracing of non-child processes by non-root users.
That is, if you want to be able to debug simulation processes by attaching to them
with a debugger, or similarly, you want to be able to use |omnet++| just-in-time
debugging (``debugger-attach-on-startup`` and ``debugger-attach-on-error``
configuration options), you need to explicitly enable them:

To temporarily allow ptracing non-child processes, enter the following command:

.. code::

   $ echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope

To permanently allow it, add

.. code::

   boot.kernel.sysctl."kernel.yama.ptrace_scope" = 0;

to the ``/etc/nixos/configuration.nix`` file and then rebuild your configuration.
