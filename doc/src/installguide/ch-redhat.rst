Red Hat Enterprise Linux (RHEL) and AlmaLinux
=============================================

Supported Releases
------------------

This chapter provides additional information for installing |omnet++| on
Red Hat Enterprise Linux (RHEL), RockyLinux and AlmaLinux distributions.

The following releases were tested, without 3D support:

-  AlmaLinux 9.x
-  AlmaLinux 10.x

RHEL, RockyLinux and CentOS Stream of the same major versions are expected to work.

Installing the Prerequisite Packages
------------------------------------

To install the required packages, type in the terminal. You may need ``sudo``
privileges for these commands.

First, enable the EPEL (Extra Packages for Enterprise Linux) repository,
which provides additional packages:

.. code::

   $ sudo dnf install -y epel-release

Then, install the core development tools and libraries:

.. code::

   $ sudo dnf install -y make ccache clang lld lldb gdb bison flex perl \
       python3-devel python3-pip libxml2-devel zlib-devel graphviz \
       xdg-utils elfutils-devel

.. note::

   Doxygen is not in this list, and ``install.sh`` does not install it either: on these distributions it is
   available only from the CodeReady Builder (CRB) repository, which is usually not enabled. Doxygen is
   needed only for generating the documentation of simulation models. If you need it, enable CRB and install it:
   ``sudo dnf config-manager --set-enabled crb && sudo dnf install -y doxygen`` (on RHEL itself, enable
   ``codeready-builder-for-rhel-<version>-<arch>-rpms`` with ``subscription-manager`` instead).

.. note::

   On RHEL/AlmaLinux/RockyLinux 9, the default Python 3.9 is too old. Install ``python3.12-devel python3.12-pip``
   instead of ``python3-devel python3-pip``, and use ``python3.12`` in place of ``python3`` in the commands below.

Next, install packages for the graphical environment (Qtenv and IDE). If you
do not need GUI support, you can skip this step and later configure |omnet++|
with ``WITH_QTENV=no`` and ``WITH_OSG=no``.

.. code::

   $ sudo dnf install -y qt6-qttools-devel qt6-qtbase-devel qt6-qtsvg qt6-qtwayland

.. warning::

   **3D Visualization (OpenSceneGraph) Support**

   OpenSceneGraph is generally **not available or easily installable** on RHEL/AlmaLinux distributions from standard repositories. Therefore, it is strongly recommended to build |omnet++| **without** 3D support on these systems.

   You should configure |omnet++| with the ``WITH_OSG=no`` option. If you are using the ``install.sh`` script, it asks whether to continue without 3D support (with ``-y``, it continues without asking); use the ``--no-3d`` flag to skip the question.

Install ``uv``, which is used to set up the Python environment. It is not packaged for this distribution, so install it
for the current user (``uv`` will be placed in ``~/.local/bin``, make sure it is on your ``PATH``):

.. code::

   $ python3 -m pip install --user uv

Next, set up a Python virtual environment for |omnet++| using ``uv``. In the root directory of your |omnet++| download:

.. code::

   $ UV_PYTHON_DOWNLOADS=never uv venv --python python3 --seed --prompt "omnetpp/.venv" .venv
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
   If you skip the GUI packages or due to the lack of OpenSceneGraph, remember to disable the corresponding features (``WITH_QTENV=no``, ``WITH_OSG=no``) in ``configure.user`` or during the ``./configure`` step.

To install additional (optional) packages for parallel simulation, type:

.. code::

   $ sudo dnf install -y openmpi-devel

Note that *openmpi* will not be available by default, it needs to be activated in every session with the

.. code::

   $ module load openmpi_<arch>

command, where ``<arch>`` is your architecture (usually ``x86_64``). When in doubt, use ``module avail`` to
display the list of available modules. If you need MPI in every session, you may add the ``module load`` command to your
startup script (``.bashrc``).

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
