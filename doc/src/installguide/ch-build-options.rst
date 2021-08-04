Build Options
=============

Configure.user Options
----------------------

The ``configure.user`` file contains several options that can be used to fine-tune the simulation libraries.

You always need to re-run the ``configure`` script in the installation root after changing the ``configure.user`` file.

.. code::

   $ ./configure

After this step, you have to remove all previous libraries and recompile |omnet++|:

.. code::

   $ make cleanall
   $ make

Options:

``PREFER_CLANG=no``
   If both ``gcc`` and ``clang`` are installed on your system, setting this variable to ``no`` will force the configure
   script to use ``gcc`` as C++ compiler.

.. ifconfig:: what=='omnest'

   ``WITH_SYSTEMC=yes``
      Use this variable to enable integration whith the bundled SystemC reference implementation.

``<COMPONENTNAME>_CFLAGS``, ``<COMPONENTNAME>_LIBS``
   The ``configure.user`` file contains variables for defining the compile and link options needed by various external
   libraries. By default, the ``configure`` command detects these automatically, but you may override the auto detection
   by specifying the values by hand. (e.g. ``<COMP>_CFLAGS=-I/path/to/comp/includedir`` and
   ``<COMP>_LIBS=-L/path/to/comp/libdir -lnameoflib``.)

``WITH_PARSIM=no``
   Use this variable to explicitly disable parallel simulation support in |omnet++|.

``WITH_NETBUILDER=no``
   This option allows you to leave out the NED language parser and the network builder. (This is needed only if you are
   building your network with C++ API calls and you do not use the built-in NED language parser at all.)

``WITH_QTENV=no``
   This will prevent the build system to link with the Qt libraries. Use this option if your platform does not have a
   suitable Qt package or you will run the simulation only in command line mode. (i.e. You want to run |omnet++| in a
   remote terminal session.)

``WITH_OSG=no``
   This will prevent the build system to use OpenScreenGraph which is used for 3D visualization in Qtenv.

``WITH_OSGEARTH=no``
   This will prevent the build system to use osgEarth which is used for 2D/3D mapping and visualization in Qtenv.

``CFLAGS_[RELEASE/DEBUG]``
   To change the compiler command line options the build process is using, you should specify them in the
   ``CFLAGS_RELEASE`` and ``CFLAGS_DEBUG`` variables. By default, the flags required for debugging or optimization are
   detected automatically by the ``configure`` script. If you set them manually, you should specify all options you
   need. It is recommended to check what options are detected automatically (check the ``Makefile.inc`` after running
   ``configure`` and look for the ``CFLAGS_[RELEASE/DEBUG]`` variables.) and add/modify those options manually in the
   ``configure.user`` file.

``LDFLAGS``
   Linker command line options can be explicitly set using this variable. It is recommended to check what options are
   detected automatically (check the ``Makefile.inc`` after running ``configure`` and look for the ``LDFLAGS``
   variable.) and add/modify those options manually in the ``configure.user`` file.

``SHARED_LIBS``
   This variable controls whether the |omnet++| build process will create static or dynamic libraries. By default, the
   |omnet++| runtime is built as a set of shared libraries. If you want to build a single executable from your
   simulation, specify ``SHARED_LIBS=no`` in ``configure.user`` to create static |omnet++| libraries and then
   reconfigure (``./configure``) and recompile |omnet++| (``make cleanall; make``). Once the |omnet++| static libraries
   are correctly built, your own project have to be rebuilt, too. You will get a single, statically linked executable,
   which requires only the NED and INI files to run.

.. warning::

   It is important to completely delete the |omnet++| libraries (``make cleanall``) and then rebuild them, otherwise it
   cannot be guaranteed that the created simulations are linked against the correct libraries.

.. note::

   The ``USE_DOUBLE_SIMTIME`` and ``WITHOUT_CPACKET`` options are no longer supported. They were introduced in |omnet++|
   4.0 to help porting model code from |omnet++| 3.x, and having fulfilled their role, they were removed in |omnet++|
   5.0. If you still have old model code to port, use |omnet++| 4.x.

Moving the Installation
-----------------------

When you build |omnet++| on your machine, several directory names are compiled into the binaries. This makes it easier
to set up |omnet++| in the first place, but if you rename the installation directory or move it to another location in
the file system, the built-in paths become invalid and the correct paths have to be supplied via environment variables.

The following environment variables are affected (in addition to ``PATH``, which also needs to be adjusted):

``OMNETPP_IMAGE_PATH``
   This variable contains the list of directories where Qtenv looks for icons. Set it to point to the ``images/``
   subdirectory of your |omnet++| installation.

``LD_LIBRARY_PATH``
   This variable contains the list of additional directories where shared libraries are looked for. Initially,
   ``LD_LIBRARY_PATH`` is not needed because shared libraries are located via the *rpath* mechanism. When you move the
   installation, you need to add the ``lib/`` subdirectory of your |omnet++| installation to ``LD_LIBRARY_PATH``.

.. note::

   On macOS, ``DYLD_LIBRARY_PATH`` is used instead of ``LD_LIBRARY_PATH``. On Windows, the ``PATH`` variable must
   contain the directory where shared libraries (DLLs) are present.

Using Different Compilers
-------------------------

By default, the configure script detects the following compilers automatically in the path:

-  Intel compiler (icc, icpc)
-  GNU C/C++ (gcc, g++)
-  Clang (clang, clang++)
-  Clang/C2 (from Microsoft Visual Studio)
-  Sun Studio (cc, cxx)
-  IBM compiler (xlc, xlC)

If you want to use compilers other than the above ones, you should specify the compiler name in the ``CC`` and ``CXX``
variables, and re-run the configuration script.

.. note::

   Different compilers may have different command line options. If you use a compiler other than the default ``gcc``,
   you may have to revise the ``CFLAGS_[RELEASE/DEBUG]`` and ``LDFLAGS`` variables.
