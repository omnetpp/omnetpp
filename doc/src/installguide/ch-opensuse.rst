OpenSUSE
========

Supported Releases
------------------

This chapter provides additional information for installing |omnet++| on openSUSE installations. The overall
installation procedure is described in the *Linux* chapter.

The following openSUSE releases are supported:

-  openSUSE Tumbleweed
-  openSUSE Leap 16.0

It was tested on the following architectures:

-  Intel 64-bit

Installing the Prerequisite Packages
------------------------------------

First, install the core development tools and libraries:

.. code::

   $ sudo zypper install -y make ccache clang lld lldb gdb bison gawk flex perl \
       python3-devel python3-pip doxygen graphviz xdg-utils \
       libdw-devel

Next, install packages for the graphical environment (Qtenv and IDE). If you do not need GUI support (e.g., for a server installation), you can skip this step and later configure |omnet++| with ``WITH_QTENV=no`` and ``WITH_OSG=no``.

.. code::

   $ sudo zypper install -y qt6-base-devel qt6-wayland libQt6Svg6 libwebkit2gtk-4_1-0

For 3D visualization support in Qtenv, install the OpenSceneGraph development packages. If you do not need 3D support, you can skip this step and later configure |omnet++| with ``WITH_OSG=no``.

.. code::

   $ sudo zypper install -y libOpenSceneGraph-devel OpenSceneGraph-plugins

After installing system packages, it's good practice to clean the local repository of retrieved package files:

.. code::

   $ sudo zypper clean

Install ``uv``, which is used to set up the Python environment. On Tumbleweed, install the ``uv`` package:

.. code::

   $ sudo zypper install -y uv

On Leap, install it for the current user (``uv`` will be placed in ``~/.local/bin``, make sure it is on your ``PATH``):

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
   If you skip the GUI or 3D packages, remember to disable the corresponding features (``WITH_QTENV=no``, ``WITH_OSG=no``) in ``configure.user`` or during the ``./configure`` step.

To enable the optional parallel simulation support you will need to install the MPI package:

.. code::

   $ sudo zypper install openmpi-devel

Note that *openmpi* will not be available by default, first you need to log out and log in again, or source your
``.profile`` script:

.. code::

   $ . ~/.profile
