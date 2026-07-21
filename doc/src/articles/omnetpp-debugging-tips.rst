:author: Rudolf Hornig
:date: 2020-07-05

Improving the C++ Debugging Experience on Linux and other OSes
==============================================================

You would think that in 2020, C++ debugging on Linux is a solved problem. C++ is a mature language used by a huge
programmer audience and a multitude of large projects. It has excellent open-source tool support, and which OS would run
those tools run better than Linux, right?

Well, yes and no. Members of our team have gone through considerable pain trying to debug during the development of the
INET Framework. While eventually we have managed to overcome the difficulties (mostly), it was far from being trivial,
and took us a lot of research to find the combination of tools, compile options and other tricks that are able to
provide an improved C++ debugging experience. In this blog post we share our experiences, in an effort to help our
fellow developers. (It is important to note that content in this post might have a limited "best before" date, as the
software landscape is changing over time in C++ land, too.)

The Issues
----------

Here are a few issues we have experienced at one time or another:

- ``std::string`` (and other standard types) displayed as ``<incomplete type>`` in the debugger
- seeing the internals of the class (instead of the data you are interested in) when inspecting an ``std::string``,
  ``std::vector`` or other standard containers
- seeing the ``<optimised out>`` message when inspecting a variable in your debug build (this happens even with ``-O0``,
  which is supposed to disable all optimizations)
- expression evaluation is incomplete (not all C++ syntax is supported) and unreliable (often reports errors, etc.)
- need for step filtering: as users, we are usually not interested in debugging into the internals of the standard C++
  classes
- linking INET with full debug info taking too long
- debugger taking a very long time to start up, due to the amount of debug info it needs to load
- debugger not stopping at breakpoints
- response times being too slow when debugging with ``gdb``
- not all debugger frontends being able to use ``lldb`` as backend
- choice of standalone debugger (there are many, all of them with their own issues)

But before we delve into the details, let's come clear with the basics.

Debugging on Various Platforms
------------------------------

A debugger usually consists of two parts: a frontend (UI), and a debugger backend. The backend is responsible for the
actual low-level debugging tasks like starting a process, setting breakpoints, stepping in the code, querying variables
etc. A frontend, on the other hand, is responsible for displaying the debugging information to the user and handling
user input.

The OMNeT++ IDE, which is based on Eclipse, contains a debugger frontend provided by the CDT project. The CDT debugger
uses ``gdb`` as its backend. ``gdb`` must be installed separately from the IDE. On Linux, ``gdb`` is normally installed
by the system's package manager. On Windows, ``gdb`` is included in the bundled MinGW tool-chain. Finally on macOS, you
must get ``gdb`` from a 3rd party package manager (like homebrew), because the ``gdb`` instance that comes with macOS is
no longer maintained and is quite outdated. (This is because macOS uses ``lldb`` as the default debugger, and they no
longer need/care about ``gdb``.) Installing ``gdb`` on macOS can be a headache, because the 3rd party ``gdb`` executable
must be digitally signed locally to be able to debug other processes. Because of the above limitations, sometimes it
makes sense to debug OMNeT++ models outside of the IDE.

.. note::
   On macOS, OMNeT++ 6 uses ``lldb`` (through the bundled ``lldb-mi2`` driver) so the above issues are no longer present and debugging is working out of box.

Debugger Backends - Pros and Cons
---------------------------------

We recommend three possible debugger backends for OMNeT++. Each of them have their own strengths and weaknesses:

- ``gdb``
    - PRO: very mature debugger, with a few unique features like, reverse debugging, continuous debugging or pretty
      printing. A lot of debugger frontends support ``gdb``.
    - CON: it can get quite slow with complicated C++ code, variable evaluation and stepping is not always reliable and
      sometimes rather slow. On macOS, setting up ``gdb`` is quite complicated and it still has limited usability.
- ``lldb``
    - PRO: a modern debugger with great pretty printing support, fast stepping and evaluation. Works out of the box on
      macOS.
    - CON: There is only a handful of frontends that support LLDB. Notably, Eclipse CDT does not support ``lldb`` at the
      moment.
- `rr <https://rr-project.org/>`_
    - PRO: This is an interesting special-purpose debugger. It supports execution recording and replaying (hence the
      name) with forward and **reverse** execution of the application. It is a ``gdb`` drop-in replacement, with a few
      additional commands for reverse execution.
    - CON: Few frontends support it properly, and ``rr`` itself works only on Intel-made CPUs made after cca. 2010.

Debugger Frontends
------------------

Here are a few debugger frontends we would like to highlight:

- Eclipse CDT

    This is the built-in debugger in the OMNeT++ IDE. It integrates only with ``gdb`` (at the moment), so it inherits
    all the advantages and disadvantages of the ``gdb`` backend.

- Visual Studio Code

    VS Code is an extensible cross platform code editor/IDE that has several debugger extensions on its Marketplace.
    Some of them supports ``gdb``, ``lldb``, or both.

    VS Code is practically the only viable cross-platform debugger frontend. The primary mode of launching debugging
    sessions in VS Code is by creating debug configurations in its own editor. Launching it from the command-line as a
    standalone debugger is also possible, but it requires crafting some nifty command-line arguments (which could be
    hidden behind a shell script, for user sanity).

- Xcode

    This is the official development environment on macOS (and only available on that platform), and uses ``lldb`` as
    its backend. Setting up a debugging session for an OMNeT++ model can be quite complicated, as you have to set up a
    workspace and a project in Xcode before debugging.

- gdbgui

    This is a Python-based frontend (for ``gdb`` and ``rr``) so it can naturally run on all platforms. While it is
    somewhat limited, its unique point is that the user interface is rendered in a browser, so you can easily debug a
    remotely running process. It has also first class integration with the ``rr`` reverse debugger, so you can easily
    navigate an execution recording. This is extremely useful to catch rarely occurring crashes and bugs.

- nemiver

    This is a GTK-based front-end for ``gdb``. It provides a reasonably complete and comfortable debugging experience on
    Linux systems, albeit the user interface has some annoying quirks. As the project has not received much attention
    from its developers for years, it is unclear when/whether those issues are going to be resolved.

- KDbg

    KDbg is roughly the KDE equivalent of Nemiver: a Qt-based frontend to ``gdb``.

Configuring OMNeT++ for Optimal Debugging Experience
----------------------------------------------------

To achieve the best possible experience during debugging, you should fine tune some OMNeT++ compiler and linker flags
before building OMNeT++ and your code in debug mode. Compiler flags can be configured in the ``configure.user`` file by
adding the necessary options to the ``CFLAGS_DEBUG`` variable and then re-running ``./configure`` and re-building
OMNeT++ and your model. You can temporarily add these options also to ``Makefile.inc``. In this case it is enough to
just clean and re-build OMNeT++ and your model without re-configuration::

    make cleanall && make MODE=debug

- Depending on whether you intend to use ``gdb`` or ``lldb`` for debugging, you should add ``-ggdb3`` or ``-glldb`` to
  the ``CFLAGS_DEBUG`` variable. Note that by default OMNeT++ assumes you intend to use gdb.

- You may need to configure ``gdb`` to improve load times if you work with big projects. In ``~/.gdbinit`` specify
  ``main set worker-threads unlimited`` to allow parallel loading of debug information.

- If you use ``clang`` as your compiler and use 3rd-party libraries in you project that do not have debugging symbols,
  you may have a hard time inspecting some types defined in those libraries. This is especially painful with the
  standard C++ library, where you cannot inspect ``std::string`` or other standard containers. This usually surfaces as
  a debugger error complaining about 'incomplete types'.

  To remedy this situation, you should install the corresponding debug symbols on your operating system (i.e. on Ubuntu:
  ``sudo apt-get install libstdc++6-8-dbg``), however note that the actual name of the debug symbol package varies
  widely.

  A more generic solution is to force the ``clang`` compiler to emit debug information for all types it encounters
  during compilation, but this greatly increases the size of the debug info (and the linking time as a consequence),
  because debug info for the standard C++ library will be included for *each* compilation unit. To force the compiler to
  generate full debug info, add ``-fstandalone-debug`` to the ``CFLAGS_DEBUG`` variable. However, this is recommended
  only if you cannot install the debugging symbols for the given library.

  .. note::
      This is not a problem with ``gcc`` and ``clang`` on macOS, where the default behavior is to generate full debug info anyway.

- If you had to apply the above workaround to get the standard C++ types working in the debugger, you may experience
  increased linking times, especially with big projects. You can specify ``-gsplit-dwarf`` in the ``CFLAGS_DEBUG``
  variable to force the debug info into a separate (``*.dwo``) file for each compilation unit. This will speed up the
  linking process, but it will impact the startup time of the debugger. In turn, debugger startup time can be improved
  by supplying the ``-Wl,--gdb-index`` option as well, which enables pre-indexing of the ``.dwo`` files, thereby
  allowing faster loading.

- It is also useful to force some additional runtime-checking to avoid hard to detect bugs like stack overflows. Add
  ``-fstack-protector`` (or ``-fsanitize=safe-stack`` if you use ``clang``) to detect these issues.

Debugging with VS Code
----------------------

VS Code has several debugger extensions, some of them supporting either ``gdb`` or ``lldb``. We found ``CodeLLDB`` the
most usable for debugging OMNeT++ models. As its name suggest, this extension uses ``lldb`` as a backend. To install,
just open VS Code's extensions view, type ``CodeLLDB`` in the search field and click the 'install' button.

As this debugger uses ``lldb``, we recommend adding the ``-glldb`` option to the ``CFLAGS_DEBUG`` variable in
``configure.user``.

Launching Simulations with the CodeLLDB debugger
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To launch as a standalone debugger, create an executable shell script named ``codelldb`` in your path, containing::

    #!/bin/sh
    PROG=$(realpath $1)
    shift
    code --open-url "vscode://vadimcn.vscode-lldb/launch/command?$PROG $*"

After that, you can start debugging with::

    codelldb ./aloha_dbg -u Cmdenv -c PureAloha2

Attaching CodeLLDB to the Simulation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is also possible to configure OMNeT++ to allow invoking the debugger from inside the simulation by choosing the
``Simulate|Debug Now`` or ``Simulate|Debug Next Event`` menu items. In this case the simulation will execute a
pre-configured command to launch the debugger and attach the current process to it. You have two options to do this:

- You can configure it globally by setting the following environment variable in your shell's startup file (i.e.
  ``.bashrc``)::

    export OMNETPP_DEBUGGER_COMMAND="code --open-url \"vscode://vadimcn.vscode-lldb/launch/config?{request: 'attach', pid: '%u'}\""

- You can set it only for the current simulation by setting the following configuration key in your ``omnetpp.ini``::

    debugger-attach-command="code --open-url \"vscode://vadimcn.vscode-lldb/launch/config?{request: 'attach', pid: '%u'}\""

Both approaches will allow you to drop into the debugger interactively from Qtenv.

Pretty Printing OMNeT++ Data Structures
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Both ``gdb`` and ``lldb`` allows you to define pretty printers in Python that allow you to better display some
complicated data structures. OMNeT++ defines such printers for certain data structures (i.e. simtime_t etc.). To
activate the pretty printers import them in the VS Code **debug console**::

    command script import <OMNETPP_ROOT>/python/omnetpp/lldb/formatters/omnetpp.py

.. note::
    Formatters for ``gdb`` and ``lldb`` are not compatible with each other!

Debugging with gdbgui
---------------------

`Gdbgui <https://www.gdbgui.com/>`_ is a browser-based graphical frontend for ``gdb`` and ``rr``, written in Python. To
install just type::

    $ pip3 install gdbgui

Start the debugger using::

    $ gdbgui --args ./aloha_dbg -u Cmdenv -c PureAloha1

which will launch also a browser window containing the debugger UI.

Reverse Debugging Tips
----------------------

Reverse debugging allows you to step/run in forward and reverse direction. It is very easy to run until an exception
happened, then set a breakpoint and run backwards until that breakpoint is reached. This will stop at the point when
last time the execution passed that point before the exception happened. It is very easy to uncover the actual cause of
the bug this way. Both ``gdbgui`` and ``VS Code`` support reverse debugging using `mozilla's rr
<https://rr-project.org/>`_ as a backend.

.. note::
    ``rr`` works only on recent Intel processors.

To record a simulation, use::

    $ rr record ./aloha_dbg -u Cmdenv -c PureAloha1

.. note::
    Graphical programs cannot be recorded properly so we recommend running your simulation in Cmdenv mode.

If you want to send a recorded simulation to an other machine, you can *pack* it, so all dependencies of the executable
will be copied into the directory, and then that directory can be compressed and sent to a different user::

    $ rr pack <RECORD_DIRECTORY>

Replaying with gdbgui
~~~~~~~~~~~~~~~~~~~~~

Start the debugger in replay mode, which will replay the last recording::

    $ gdbgui --rr

or an earlier recording stored in a directory::

    $ gdbgui DIRECTORY --rr

Replaying with VS Code
~~~~~~~~~~~~~~~~~~~~~~

First, start a replay::

    $ rr replay -s 50001

then the VS Code editor using the same port number::

    $ code --open-url "vscode://vadimcn.vscode-lldb/launch/config?{targetCreateCommands: ['target create full/path/to/omnetpp/samples/aloha/aloha_dbg'], processCreateCommands: ['gdb-remote 127.0.0.1:50001'], request: 'custom', reverseDebugging: true }"

You will see some extra buttons on the debugger toolbar allowing reverse stepping and execution.
