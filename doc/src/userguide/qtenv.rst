The Qtenv Graphical Runtime Environment
=======================================

Qtenv is a graphical runtime interface for simulations. It supports interactive simulation execution, animation,
inspection, tracing, and debugging. In addition to model development and verification, Qtenv is also useful for
presentation and educational purposes, as it allows the user to get a detailed picture of the state and history of the
simulation at any point of its execution.

Overview of the User Interface
------------------------------

.. figure:: pictures/Qtenv-main.png
   :width: 80%

   The main window of Qtenv

The top of the window contains the following elements below the menu bar:

-  :guilabel:`Toolbar`: The toolbar lets you access the most frequently used functions, such as stepping, running and
   stopping the simulation.
-  :guilabel:`Animation speed`: The slider at the end of the toolbar lets you scale the speed of the built-in
   animations, as well as the playback speed of the custom animations added by the model.
-  :guilabel:`Event Number and Simulation Time`: These two labels at the right end of the toolbar display the event
   number of the last executed or the next future event, and the current simulation time. The display format can be
   changed from the context menu.
-  :guilabel:`Top status bar`: Three labels in a row that display either information about the next simulation event (in
   :guilabel:`Step` and :guilabel:`Normal` mode), or performance data like the number of events processed per second (in
   :guilabel:`Fast` and :guilabel:`Express` mode). This can be hidden to free up vertical space.
-  :guilabel:`Timeline`: Displays the contents of the Future Events Set (FES) on a logarithmic time scale. The timeline
   can be turned off to free up vertical space.
-  :guilabel:`Bottom status bar`: Displays the current configuration, the run number, and the name of the root module
   (network) on the left, and a few statistics about the message objects in the model on the right.

The central area of the main window is divided into the following regions:

-  :guilabel:`Object Navigator`: Displays the hierarchy of objects in the current simulation and in the FES.
-  :guilabel:`Object Inspector`: Displays the contents and properties of the selected object.
-  :guilabel:`Network Display`: Displays the network or any module graphically. This is also where animation takes
   place.
-  :guilabel:`Log Viewer`: Displays the log of packets or messages sent between modules, or log messages output by
   modules during simulation.

Additionally, you can open inspector windows that float on top of the main window.

.. note::

   If you experience visual issues such as colors that hinder using Qtenv, you can disable the platform integration style
   plugins of Qt by setting the ``QT_STYLE_OVERRIDE`` environment variable to ``fusion``. This will make the widgets appear in a
   platform-independent manner. For example, on Linux systems with some desktop environments, you might experience a
   white-on-white color scheme; setting ``QT_STYLE_OVERRIDE`` to ``fusion`` will make the colors default to a light gray
   background with dark gray text.

Parts of the Qtenv UI
---------------------

This section describes in detail the various parts of the Qtenv UI and how they function.

The Status Bars
~~~~~~~~~~~~~~~

The status bars show the simulation's progress. There is one row at the top of the main window, and one at the bottom.
The top one can be hidden using the :menuselection:`View --> Status Details` menu item.

.. figure:: pictures/Qtenv-statusbar.png
   :width: 90%

   The top status bar

When the simulation is paused or runs with animation, the top row displays the next expected simulation event. Note the
word :guilabel:`expected`. Certain schedulers may insert new events before the displayed event at the last moment. Some
schedulers that tend to do that are those that accept input from outside sources: real-time scheduler, hybrid or
hardware-in-the-loop schedulers, parallel simulation schedulers, etc. The top row contains the following:

1. Name, C++ class, and ID of the next message (event) object
2. The module where the next event will occur (i.e., the module where the message will be delivered)
3. The simulation time of the next (expected) simulation event
4. Time of the next event, and delta from the current simulation time

When the simulation is running in :guilabel:`Fast` or :guilabel:`Express` mode, displaying the next event becomes
useless. The contents of the top row are replaced by the following performance gauges:

.. figure:: pictures/Qtenv-statusbar-running.png
   :width: 90%

   The top status bar during Fast or Express run

1. Simulation speed: number of events processed per real second
2. Relative speed of the simulation (compared to real-time)
3. Event density: the number of events per simulated second

The bottom row contains the following items:

.. figure:: pictures/Qtenv-statusbar-bottom.png
   :width: 90%

   The bottom status bar

1. Ini config name, run number, and the name of the network
2. Message statistics: the number of messages currently scheduled (i.e., in the FES); the number of message objects that
   currently exist in the simulation; and the number of message objects that have been created so far, including the
   already deleted ones. Out of the three, the middle one is probably the most useful. If it is steadily growing without
   apparent reason, the simulation model is probably missing some ``delete msg`` statements and needs debugging.

The Timeline
~~~~~~~~~~~~

.. figure:: pictures/Qtenv-timeline.png
   :width: 90%

   The timeline

The timeline displays the contents of the Future Events Set on a logarithmic time scale. Each dot represents a message
(event). Messages to be delivered in the current simulation time are grouped into a separate section on the left between
brackets.

Clicking an event will focus it in the :guilabel:`Object Inspector`, and double-clicking it will open a floating inspector
window. Right-clicking it will bring up a context menu with further actions.

The timeline is often crowded, limiting its usefulness. To overcome this, you can hide uninteresting events from the
timeline. Right-click the event and choose :guilabel:`Exclude Messages Like 'x' From Animation` from the context menu.
This will hide events with a similar name and the same C++ class name from the timeline, and also skip the animation when
such messages are sent from one module to another. You can view and edit the list of excluded messages on the
:guilabel:`Filtering` page of the :guilabel:`Preferences` dialog. (Tip: the timeline context menu provides a shortcut to
that dialog).

The whole timeline can be hidden (and revealed again) using the :menuselection:`View --> Timeline` menu item, by pressing a
button on the toolbar, or simply by dragging the handle of the separator under it all the way up.

The Object Navigator
~~~~~~~~~~~~~~~~~~~~

.. figure:: pictures/Qtenv-objtree.png
   :width: 40%

   The object tree

The :guilabel:`Object Navigator` displays the hierarchy of objects in the current simulation and in the FES in a tree form.

Clicking an object will focus it in the :guilabel:`Object Inspector`, and double-clicking it will open a floating inspector
window. Right-clicking it will bring up a context menu with further actions.

The Object Inspector
~~~~~~~~~~~~~~~~~~~~

The :guilabel:`Object Inspector` is located below the :guilabel:`Object Navigator` and lets you examine the contents of
objects in detail. The :guilabel:`Object Inspector` always focuses on the object last clicked or selected on
the Qtenv UI. It can also be navigated directly using the :guilabel:`Back`, :guilabel:`Forward`, and :guilabel:`Go to
Parent` buttons, and by double-clicking objects shown inside the inspector's area.

.. list-table::

   * - .. figure:: pictures/Qtenv-inspector-grouped.png
          :width: 80%

          The object inspector in Grouped mode

     - .. figure:: pictures/Qtenv-inspector-flat.png
          :width: 80%

          The object inspector in Flat mode

The inspector has four display modes: :guilabel:`Grouped`, :guilabel:`Flat`, :guilabel:`Children`, and
:guilabel:`Inheritance`. You can switch between these modes using the buttons on the inspector's toolbar.

In :guilabel:`Grouped`, :guilabel:`Flat`, and :guilabel:`Inheritance` modes, the tree shows the fields (or data members)
of the object. It uses meta-information generated by the message compiler to obtain the list of fields and their values.
(This is true even for the built-in classes -- the simulation kernel contains their description of msg format.)

The only difference between these three modes is the way the fields are arranged. In :guilabel:`Grouped` mode, they are
organized in categories; in :guilabel:`Flat` mode, they form a simple alphabetical list; and in :guilabel:`Inheritance`
mode, they are organized based on which superclass they are inherited from.

.. list-table::

   * - .. figure:: pictures/Qtenv-inspector-children.png
          :width: 80%

          The object inspector in Children mode

     - .. figure:: pictures/Qtenv-inspector-inheritance.png
          :width: 80%

          The object inspector in Inheritance mode

In :guilabel:`Children` mode, the tree shows the child objects of the currently inspected object. The child list is
obtained via the ``forEachChild()`` method of the object. This is very similar to how the :guilabel:`Object Navigator`
works, but this can have an arbitrary root.

The Network Display
~~~~~~~~~~~~~~~~~~~

The network view provides a graphical view of the network and modules in general. The graphical representation is based on
display strings (``@display`` properties in the NED file). You can go into any compound module by double-clicking its
icon.

Message sending, method calls, and certain other events are animated in the graphical view. You can customize animation
in the :guilabel:`Animation` page of the :guilabel:`Preferences` dialog.

.. figure:: pictures/Qtenv-network.png
   :width: 80%

   The network display

The built-in ``cCanvas`` of the inspected object is also rendered in this view together with the module contents to
allow overlaying custom annotations and animations. This canvas contains the figures declared by the ``@figure``
properties in the NED source of the module.

By choosing the :guilabel:`Show/Hide Canvas Layers` item in the context menu of the inspected module, you can filter the
displayed figures based on the tags set on them.

.. figure:: pictures/Qtenv-figure-filter.png
   :width: 60%

   Figure filtering dialog

Since any figure can have any number of tags, a two-step filtering mechanism is applied to provide sufficient control. The
left side is a whitelist, while the right side is a blacklist. The example above would only let all the figures with the
"oval" tag appear, except those that also have the "green" tag on them.

If the inspected module has a built-in ``cOsgCanvas`` (and Qtenv is built with OSG support enabled), this inspector can
also be switched into a 3D display mode with the globe icon on its toolbar. In this case, the 2D network and canvas
display is replaced by the scene encapsulated by the ``cOsgCanvas``.

.. figure:: pictures/Qtenv-osg.png
   :width: 80%

   The network display in 3D mode

The context menu of submodules provides further actions (see below).

.. figure:: pictures/Qtenv-submod-contextmenu.png
   :width: 60%

   Submodule context menu

.. figure:: pictures/Qtenv-submod-utilities.png
   :width: 80%

   The Utilities submenu

Zooming and Panning
^^^^^^^^^^^^^^^^^^^

There are several ways to zoom the canvas, both using the mouse and the keyboard:

-  To zoom in around a point, double-click the canvas; use Shift + double-click to zoom out or scroll while holding
   down Ctrl.

   You can also zoom around the center of the viewport with the looking glass buttons on the canvas toolbar.

-  For marquee zoom, drag out a rectangle with the left mouse button while holding down Ctrl; you can cancel the
   operation with the right mouse button.

-  Panning: moving the mouse while holding down the left mouse button will move the canvas; this is often a more
   comfortable way to navigate the canvas than using the scroll bars. You can of course scroll in any direction with
   simply the mouse wheel or the similar functionality of many touchpads.

The Log Viewer
~~~~~~~~~~~~~~

When you run the simulation, the :guilabel:`Log Viewer` lets you review the log and message traffic from the simulation.

The :guilabel:`Log Viewer` has two modes that you can toggle using the inspector's local toolbar:
:guilabel:`Messages` and :guilabel:`Log` mode.

- In :guilabel:`Messages` mode, the window displays messages/packets sent locally in a tabular format. Only messages
  sent among the submodules of the inspected module and messages sent out of/into the module are shown. You can view
  details about any message in the :guilabel:`Object Inspector` by clicking on it, and access additional functions in
  its context menu. Columns can be contributed by the simulation model via defining message printers (see C++ section in
  this chapter).

- In :guilabel:`Log` mode, the window displays log lines that belong to any module in the submodule tree of the
  inspected compound module. Log is written by simulation modules using ``EV``, ``EV_INFO``, ``EV_DETAIL``, and other
  logging macros. Formatting via ANSI escape sequences is supported. The log prefix can be configured using the
  :guilabel:`Preferences` dialog.

The embedded :guilabel:`Log Viewer` follows the selection of module inspected in the :guilabel:`Network Display`.

The :guilabel:`Log Viewer` offers useful tools like bookmarking, textual line filtering, and the :guilabel:`Set Sending
Time as Reference` option that makes it possible to set a reference time, and display all other times as a delta
compared to it.

Log is recorded only in Step, Run, and Fast Run modes. (Express mode can be so fast because such overhead is turned off
while it's active.)

Only the output from the last N events is preserved (N being configurable in the :guilabel:`Preferences` dialog), i.e.
after a while, information from older events is being discarded.

Note that it is also possible to open further :guilabel:`Log Viewers` as floating windows, allowing you to see the logs
of several modules at the same time.  To open a log window, find the module in the module tree or the network display,
right-click it, and choose :guilabel:`Open Component Log` from the context menu.

.. figure:: pictures/Qtenv-log-msgs.png
   :width: 80%

   The log viewer showing message traffic

.. figure:: pictures/Qtenv-log-textual.png
   :width: 80%

   The log viewer showing module log

You can filter the content of the window to only include messages from specific modules. Open the log window's context
menu and select :guilabel:`Filter Window Contents`.

.. figure:: pictures/Qtenv-log-filter.png
   :width: 50%

   The log filter dialog

General logging behavior, such as the prefix format, can be controlled in the :guilabel:`Preferences` dialog. The log
level of each module (and its descendants) can be set in its context menu.


Using Qtenv
-----------

Starting Qtenv
~~~~~~~~~~~~~~

When you launch a simulation from the IDE, it will be started with Qtenv by default. When it does not, you can
explicitly select Qtenv in the :guilabel:`Run` or :guilabel:`Debug` dialog.

Qtenv is also the default when you start the simulation from the command line. When necessary, you can force Qtenv by
adding the ``-u Qtenv`` switch to the command line.

The complete list of command-line options, related environment variables, and configuration options can be found at the
end of this chapter.

Setting Up and Running the Simulation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

On startup, Qtenv reads the ini file(s) specified on the command line (or ``omnetpp.ini`` if none is specified), and
automatically sets up the simulation described in them. If they contain several simulation configurations, Qtenv will
ask you which one you want to set up.

.. figure:: pictures/Qtenv-setup-dialog.png
   :width: 60%

   Setting Up a New Simulation

Once a simulation has been set up (modules have been created and initialized), you can run it in various modes and
examine its state. You can restart the simulation at any time, or set up another simulation. If you choose to quit Qtenv
before the simulation finishes (or try to restart the simulation), Qtenv will ask you whether to finalize the
simulation, which usually translates to saving summary statistics.

Functions related to setting up a simulation are in the :guilabel:`File` and :guilabel:`Simulate` menus,
and the most important ones are accessible via toolbar icons and keyboard shortcuts.

Some of these functions are:

.. figure:: pictures/Qtenv-file-menu.png
   :width: 50%

   The File menu

Set up a Configuration
^^^^^^^^^^^^^^^^^^^^^^

This function lets you choose a configuration and run number from the ini file.

Open Primary Ini File
^^^^^^^^^^^^^^^^^^^^^

Opens the first ini file in a text window for viewing.

.. figure:: pictures/Qtenv-simulate-menu.png
   :width: 50%

   The Simulate menu

Step
^^^^

:guilabel:`Step` lets you execute one simulation event, which is at the front of the FES. The next event is always shown on
the status bar. The module where the next event will be delivered is highlighted with a red rectangle on the graphical
display.

Run (or Normal Run)
^^^^^^^^^^^^^^^^^^^

In :guilabel:`Run` mode, the simulation runs with all tracing aids on. Message animation is active, and the simulation time is
interpolated if the model requested a non-zero animation speed. Inspector windows are constantly updated. Output
messages are displayed in the main window and module output windows. You can stop the simulation with the
:guilabel:`Stop` button on the toolbar. You can fully interact with the user interface while the simulation is running,
such as opening inspectors.

.. note::

   If you find this mode too slow or distracting, you may switch off animation features in the :guilabel:`Preferences`
   dialog.

Fast Run
^^^^^^^^

In :guilabel:`Fast` mode, message animation is turned off. The inspectors are updated much less often. Fast mode is
several times faster than the Run mode; the speed can increase by up to 10 times (or up to the configured event count).

Express Run
^^^^^^^^^^^

In :guilabel:`Express` mode, the simulation runs at about the same speed as with Cmdenv, with all tracing disabled. Module
log is not recorded. The simulation can only be interacted with once in a while, so the run-time overhead of the user
interface is minimal. UI updates can even be disabled completely, in which case you have to explicitly click the
:guilabel:`Update now` button to refresh the inspectors.

Run Until
^^^^^^^^^

You can run the simulation until a specified simulation time, event number, or until a specific message has been
delivered or canceled. This is a valuable tool during debugging sessions.
It is also possible to right-click on an event in the simulation timeline and choose the :guilabel:`Run until this
event` menu item.

.. figure:: pictures/Qtenv-rununtil.png
   :width: 60%

   The Run Until dialog

Run Until Next Event
^^^^^^^^^^^^^^^^^^^^

It is also possible to run until an event occurs in a specified module. Browse for the module and choose :guilabel:`Run
until next event in this module.` Simulation will stop once an event occurs in the selected module.

Conclude Simulation
^^^^^^^^^^^^^^^^^^^

This function finalizes the simulation by invoking the user-supplied ``finish()`` member functions on all module and
channel objects in the simulation. The customary implementation of ``finish()`` is to record summary statistics. The
simulation cannot be continued afterwards.

Rebuild Network
^^^^^^^^^^^^^^^

Rebuilds the simulation by deleting the current network and setting it up again. Improperly written simulations often
crash when :guilabel:`Rebuild Network` is invoked, usually due to incorrectly written destructors in module
classes.

Inspecting Objects
------------------

Inspecting Simulation Objects
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Inspectors
^^^^^^^^^^

The :guilabel:`Network Display`, the :guilabel:`Log Viewer`, and the :guilabel:`Object Inspector` in the main window
share some common properties: they display various aspects (graphical view / log messages / fields or contents) of a
given object. Such UI parts are called :guilabel:`inspectors` in Qtenv.

The three inspectors mentioned above are built into the main window, but you can open additional ones at any time.
The new inspectors will open in floating windows above the main window, and you can have any number of them open.

Inspectors come in many flavors. They can be graphical like the network view, textual like the log viewer, tree-based
like the object inspector, or something entirely different.

The screenshot below shows Qtenv with several inspectors open.

.. figure:: pictures/Qtenv-with-inspectors.png
   :width: 80%

   Qtenv with several floating inspectors open

.. note::

   Some window managers might disable/hide the close button of floating inspectors. If this happens, you can still close
   them with a keyboard shortcut (most commonly
   Alt
   +
   F4
   ), or by right-clicking on the title bar, and choosing the Close option in the appearing menu.

Opening Inspectors
^^^^^^^^^^^^^^^^^^

Inspectors can be opened in various ways: by double-clicking an item in the :guilabel:`Object Navigator` or in other
inspectors; by choosing one of the :guilabel:`Open` menu items from the context menu of an object displayed on the
UI; via the :guilabel:`Find/Inspect Objects` dialog (see later). Inspector-related menu items are in the :guilabel:`Inspect` menu.

.. figure:: pictures/Qtenv-inspect-menu.png
   :width: 50%

   The Inspect menu

There is also an :guilabel:`Inspect by pointer` menu item, which allows you to directly enter the C++ pointer of an
object as a hexadecimal value.  Beware that entering an invalid pointer will crash the simulation. Object addresses can
be obtained e.g. by using a debugger.

Navigation History
^^^^^^^^^^^^^^^^^^

Inspectors always show some aspect of one simulation object, but they can change objects. For example, in the
:guilabel:`Network View`, when you double-click a submodule that is itself a compound module, the view will switch to
showing the internals of that module; or, the :guilabel:`Object Inspector` will always show information about the object
last clicked in the UI. Inspectors maintain a navigable history: the :guilabel:`Back`/:guilabel:`Forward` functions go
to the object inspected before/after the currently displayed object. Objects that are deleted during simulation also
disappear from the history.

Restoring Inspectors
^^^^^^^^^^^^^^^^^^^^

When you exit and then restart a simulation program, Qtenv attempts to reopen the inspector windows that were open
before. However, because object identity is not preserved across different runs of the same program, Qtenv relies on
the object's full path, class name, and object ID (if available) to locate and identify the object to be inspected. This
method can occasionally result in misidentification of objects.

Preferences such as zoom level or open/closed state of a tree node are usually maintained per object type (i.e. tied to
the C++ class of the inspected object).

Browsing the Registered Components
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. figure:: pictures/Qtenv-inspect-menu-full.png
   :width: 60%

   The Inspect menu

Registered components (NED Types, classes, functions, enums) can be displayed with the :menuselection:`Inspect --> Available
components` menu item. If an error message reports missing types or classes, you can check here whether the missing item
is in fact available, i.e., registered correctly.

Querying Objects
~~~~~~~~~~~~~~~~

The :guilabel:`Find/Inspect Objects` dialog allows you to search the simulation for objects that meet certain criteria.
The criteria can be the object name, class name, the value of a field of the object, or a combination of those. Class
names must be fully qualified (i.e. they should contain the namespace(s) they are in), regardless of the related setting
in the :guilabel:`Preferences dialog`. The results are presented in a table that can be sorted by columns, and items in
the table can be double-clicked to inspect them.

Some possible use cases:

- Identifying bottlenecks in the network by looking at the list of all queues and ordering them by length (i.e. having
  the result table sorted by the :guilabel:`Info` column)
- Finding nodes with the highest packet drop count. If the drop counts are watched variables (see the ``WATCH()``
  macro), you can get a list of them.
- Finding modules that leak messages. If the live message count on the status bar keeps increasing, you can search for
  all message objects and see where the leaked messages are hiding.
- Easy access to some data structures or objects, such as routing tables. You can search by name or class name and use
  the result list as a collection of hotlinks, saving you from manually navigating the simulation's object tree.

.. figure:: pictures/Qtenv-find.png
   :width: 60%

   Using the Find/Inspect Objects dialog to find long queues

The dialog allows you to specify the search root, as well as the name and class name of the objects to find. The latter
two accept wildcard patterns.

The checkboxes in the dialog can be used to select the object categories that interest you. If you select a category,
all objects of that type (and any types derived from it) will be included in the search. Alternatively, if you specify
an object class as a class filter expression, the search dialog will try to match the object's class name with the given
string. Objects of derived types will not be included in the search.

You can also provide a generic filter expression, which matches the object's full path by default. Wildcards (``"?"``,
``"*"``) are allowed. ``"{a-exz}"`` matches any character in the range ``"a"`` through ``"e"`` and the characters
``"x"`` and ``"z"``. You can match numbers using patterns like ``"*.job{128..191}"`` to match objects named
``"job128"``, ``"job129"``, and ``"job191"``. You can also use patterns like ``"job{128..}"`` and ``"job{..191}"``.
Patterns can be combined using ``AND``, ``OR``, ``NOT``, and parentheses. Lowercase versions of these keywords (``and``,
``or``, ``not``) are also accepted. You can match other object fields, such as queue length, message kind, etc., using
the syntax ``fieldname =~ pattern``. If the pattern contains special characters or spaces, you need to enclose it in
quotes. (HINT: In most cases, you will want to start the pattern with ``"*."`` to match objects anywhere in the
network!).

Examples:

- ``*.destAddr``: Matches all objects with the name ``"destAddr"`` (likely module parameters).
- ``*.node[8..10].*``: Matches anything inside module ``node[8]``, ``node[9]``, and ``node[10]``.
- ``className =~ omnetpp::cQueue AND NOT length =~ 0``: Matches non-empty queue objects.
- ``className =~ omnetpp::cQueue AND length =~ {10..}``: Matches queue objects with length greater than or equal to 10.
- ``kind =~ 3 OR kind =~ {7..9}``: Matches messages with message kind equal to 3, 7, 8, or 9 (only messages have a ``"kind"`` attribute).
- ``className =~ IP* AND *.data-*``: Matches objects whose class name starts with ``"IP"`` and name starts with ``"data-"``.
- ``NOT className =~ omnetpp::cMessage AND byteLength =~ {1500..}``: Matches messages whose class is not cMessage and byteLength is at least 1500 (only messages have a ``"byteLength"`` attribute).
- ``"TCP packet" OR "*.packet(15)"``: Quotation marks are needed when the pattern is a reserved word or contains whitespace or special characters.


Using Qtenv with a Debugger
---------------------------

You can use Qtenv in combination with a C++ debugger, which is mainly useful when developing new models.
Qtenv can assist the user in debugging in various ways:

- By triggering a debug trap (a.k.a. debug breakpoint) at strategic places, allowing the user to
  debug a certain event or error condition.

- By launching an external debugger and attaching it to the simulation process before a debug trap is executed. Qtenv
  detects if a debugger is already attached, and if not, it offers to launch and attach one.

- By displaying memory addresses of inspected objects, to facilitate examining those objects in a debugger.

What is a debug trap? A debug trap is a mechanism used to pause the execution of a program at a specific point,
allowing developers to inspect the current state of the program, including variables, memory, and control flow. When a
debug trap is triggered, it causes the debugger to halt the program, providing an opportunity to perform detailed
analysis and debugging tasks. This is particularly useful for identifying and resolving issues such as runtime errors,
unexpected behavior, or logic flaws in the code. Debug traps can be implemented using various techniques, such as
inserting specific instructions like ``int3`` or ``SIGTRAP`` in the code, which are recognized by the debugger as
breakpoints. When no debugger is attached, a debug trap usually causes the program to abort.

.. note::

   Qtenv is a library that runs as part of the simulation program.  Therefore, suspending the simulation program in a
   debugger will also freeze the GUI until it is resumed. Also, if the simulation crashes, it will bring down the whole
   process including the Qtenv GUI.

The debugger command line to be run by Qtenv can be specified in one of two ways:

- The ``OMNETPP_DEBUGGER_COMMAND`` environment variable.
- The ``debugger-attach-command`` configuration option.

The value of these options is a string that must contain exactly one ``%u`` string that will be
replaced by Qtenv with the process ID of the simulation.

Qtenv offers the following debugging-related actions:

Debug On Errors
~~~~~~~~~~~~~~~

This menu item toggles the flag that controls whether a debug breakpoint is executed when the simulation encounters a
runtime error. This option corresponds to the ``debug-on-errors`` configuration option. The state of this menu item is
reset to the value of ``debug-on-errors`` every time a new simulation is started.

Debug Next Event
~~~~~~~~~~~~~~~~

Performs one simulation event just like :guilabel:`Step`, but executes a debugger trap just before entering the module's
event handling code (``handleMessage()`` or ``activity()``). This will cause the debugger to stop the program there,
allowing you to examine state variables, single-step, etc. When you resume execution, Qtenv will regain control and
become responsive again.

Debug Now
~~~~~~~~~

Triggers a debug trap immediately.


Recording the Simulation
------------------------

Recording an Event Log
~~~~~~~~~~~~~~~~~~~~~~

The |omnet++| simulation kernel allows you to record event-related information into a file, which can later be used to
analyze the simulation run using the :guilabel:`Sequence Chart` tool in the IDE. Eventlog recording can be turned on
with the ``record-eventlog=true`` ini file option, but also interactively, via the respective item in the
:guilabel:`Simulate` menu, or using a toolbar button.

Note that starting Qtenv with ``record-eventlog=true`` and turning on recording later does not result in exactly the
same eventlog file. In the former case, all steps of setting up the network, such as module creations, are recorded as
they happen; while for the latter, Qtenv has to "fake" a chain of steps that would result in the current state of the
simulation.

Capturing a Video
~~~~~~~~~~~~~~~~~~

When active, this feature will save the contents of the main window into a subfolder named ``frames`` in the working
directory with a regular frequency (in animation time). Each frame is a PNG image, with a sequence number in its file
name. Currently, the user has to convert (encode) these images into a video file after the fact by using an external tool
(such as ``ffmpeg``, ``avconv``, or ``vlc``). When the recording is started, an info dialog pops up, showing further
details on the output, and an example command for encoding in high quality using ``ffmpeg``. The resulting video is also
affected by the speed slider on the toolbar.

.. note::

   This built-in recording feature is able to produce a smooth video, in contrast to external screen-capture utilities.
   This is possible because it has access to more information and has more control over the process than external
   tools.

The Preferences Dialog
----------------------

Select :menuselection:`File --> Preferences` from the menu to display the runtime environment's configuration dialog.
The dialog allows you to adjust various display, network layouting, and animation options.

General
~~~~~~~

.. figure:: pictures/Qtenv-pref-general.png
   :width: 60%

   General settings

The :guilabel:`General` tab can be used to set the default user interface behavior. You can choose whether namespaces
should be stripped off the displayed class names, and how often the user interface should be updated while the
simulation runs in :guilabel:`Express` mode.

Logs
~~~~

.. figure:: pictures/Qtenv-pref-logs.png
   :width: 60%

   Logging settings

The :guilabel:`Logs` tab can be used to set the default logging behavior, such as the log level of modules that do not
override it, the prefix format for event banners, and the size limit of the log buffer.

Configuring the Layouting Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. figure:: pictures/Qtenv-pref-layouting.png
   :width: 60%

   Layouting settings

Qtenv provides automatic layouting for submodules that do not have their locations specified in the NED files. The
layouting algorithm can be fine-tuned on the :guilabel:`Layouting` page of this dialog.

Configuring Animation
~~~~~~~~~~~~~~~~~~~~~

.. figure:: pictures/Qtenv-pref-animation.png
   :width: 60%

   Animation settings

Qtenv provides automatic animation when you run the simulation. You can fine-tune the animation settings using the
:guilabel:`Animation` page of the settings dialog. If you do not need all of the visual feedback that Qtenv provides,
you can selectively turn off some of the features:

- Animate messages: Turns on/off the visualization of messages passing between modules.
- Broadcast animation: Handles message broadcasts in a special way (zero-time messages sent within the same event will
  be animated concurrently).
- Show next event marker: Highlights the module that will receive the next event.
- Show a dotted arrow when a ``sendDirect()`` method call is executed.
- Show a flashing arrow when a method call occurs from one module to another. The call is only animated if the called
  method contains the ``Enter_Method()`` macro.
- The display of message names and classes can also be turned off.

Timeline and Animation Filtering
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. figure:: pictures/Qtenv-pref-filtering.png
   :width: 60%

   Filtering

The :guilabel:`Filtering` page of the dialog serves two purposes. First, it allows you to filter the contents of the
:guilabel:`Timeline`. You can hide all self-messages (timers) or all non-self messages. Additionally, you can further
reduce the number of messages shown on the timeline by hiding the non-animated messages, as explained below.

Second, you can suppress the animation of certain messages. For example, when you are focused on routing protocol
messages, you can suppress the animation of data traffic.

The text box allows you to specify multiple filters, with each filter on a separate line. You can filter messages by
name, class name, or any other property that appears in the :guilabel:`Fields` page of the :guilabel:`Object Inspector`
when focusing it on the given message object.

.. note::

   When you select :guilabel:`Exclude Messages Like 'x' From Animation` from the context menu of a message object in the
   UI, it will add a new filter on this dialog page.

For object names, you can use wildcards (``"?"``, ``"*"``). ``"{a-exz}"`` matches any character in the range ``"a"``
through ``"e"`` and the characters ``"x"`` and ``"z"``. You can match numbers using patterns like ``"job{128..191}"`` to
match ``"job128"``, ``"job129"``, and ``"job191"``. You can also use patterns like ``"job{128..}"`` and
``"job{..191}"``. Patterns can be combined using ``AND``, ``OR``, ``NOT``, and parentheses. Lowercase versions of these
keywords (``and``, ``or``, ``not``) are also accepted. You can match against other object fields, such as message
length, message kind, etc., using the syntax ``fieldname =~ pattern``. If the pattern contains special characters or
spaces, you need to enclose it in quotes.

Some examples:

- ``m*``: Matches any object whose name begins with "m".
- ``m* AND *-{0..250}``: Matches any object whose name begins with "m" and ends with a dash and a number between 0 and 250.
- ``NOT *timer*``: Matches any object whose name does not contain the substring "timer".
- ``NOT (*timer* OR *timeout*)``: Matches any object whose name contains neither "timer" nor "timeout".
- ``kind =~ 3 OR kind =~ {7..9}``: Matches messages with message kind equal to 3, 7, 8, or 9.
- ``className =~ IP* AND data-*``: Matches objects whose class name starts with "IP" and name starts with "data-".
- ``NOT className =~ omnetpp::cMessage AND byteLength =~ {1500..}``: Matches objects whose class is not cMessage and whose byteLength is at least 1500.
- ``"TCP packet" OR "*.packet(15)"``: Quotation marks are needed when the pattern is a reserved word or contains whitespace or special characters.

There is also a per-module setting that models can adjust programmatically to prevent any animations from happening when
inspecting a given module (``setBuiltinAnimationsAllowed()``).

Configuring Fonts
~~~~~~~~~~~~~~~~~

.. figure:: pictures/Qtenv-pref-fonts.png
   :width: 60%

   Font selection

The :guilabel:`Fonts` page of the settings dialog allows you to select the typeface and font size for various user
interface elements.

The .qtenvrc File
~~~~~~~~~~~~~~~~~

Settings are stored in ``.qtenvrc`` files. There are two ``.qtenvrc`` files: one is stored in the current directory and
contains project-specific settings, such as the list of open inspectors; the other is saved in the user's home directory
and contains global settings.

.. note::

   Inspectors are identified by their object names. If you have several components that share the same name (this is
   especially common for messages), you may end up with a lot of inspector windows when you start the simulation. In
   such cases, you can simply delete the ``.qtenvrc`` file.

Qtenv and C++
-------------

This section describes which C++ API functions various parts of Qtenv use to display data and perform their functions.
Most functions are member functions of the ``cObject`` class.

Inspectors
~~~~~~~~~~

Inspectors display the hierarchical name (i.e., full path) and class name of the inspected object in the title using the
``getFullPath()`` and ``getClassName()`` member functions of ``cObject``. The :guilabel:`Go to parent` feature in
inspectors uses the ``getOwner()`` method of ``cObject``.

The :guilabel:`Object Navigator` displays the full name and class name of each object (``getFullName()`` and
``getClassName()``), and also the ID for classes that have one (``getId()`` on ``cMessage`` and ``cModule``). When you
hover with the mouse, the tooltip displays the info string (``str()`` method). The roots of the tree are the network
module (``simulation.getSystemModule()``) and the FES (``simulation.getFES()``). Child objects are enumerated with the
help of the ``forEachChild()`` method.

The :guilabel:`Object Inspector` in :guilabel:`Children` mode displays the full name, class name, and info string
(``getFullName()``, ``getClassName()``, ``str()``) of child objects enumerated using ``forEachChild()``.
``forEachChild()`` can only enumerate objects that are subclasses of ``cObject``. If you want non-``cObject`` variables
(e.g., primitive types or STL containers) to appear in the :guilabel:`Children` tree, you need to wrap them into
``cObject``. The ``WATCH()`` macro does exactly that: it creates an object wrapper that displays the variable's value
via the wrapper's ``str()`` method. There are also watch macros for STL containers; they present the wrapped object to
Qtenv in a more structured way using custom class descriptors (cClassDescriptor, see below).

One might wonder how the ``forEachChild()`` method of modules can enumerate messages, queues, and other objects owned by
the module. The answer is that the module class maintains a list of owned objects, and ``cObject`` automatically joins
that list.

The :guilabel:`Object Inspector` displays an object's fields by making use of the class descriptor
(``cClassDescriptor``) for that class. Class descriptors are automatically generated for new classes by the message
compiler. Class descriptors for the |omnet++| library classes are also generated by the message compiler; see
``src/sim/sim_std.msg`` in the source tree.

The :guilabel:`Network Display` uses ``cSubmoduleIterator`` to enumerate submodules, and its :guilabel:`Go to parent
module` function uses ``getParentModule()``. Background and submodule rendering is based on display strings
(``getDisplayString()`` method of ``cComponent``).

Log Viewer
~~~~~~~~~~

The module log page of :guilabel:`Log Viewer` displays the output to ``EV`` streams from modules and channels.
Note that the log viewer supports basic formatting (colors) via ANSI escape sequences.

The message/packet traffic page of :guilabel:`Log Viewer` shows information based on stored copies of sent messages (the
copy is created using ``dup()``) and stored sendhop information. The :guilabel:`Name` column displays the message name
(``getFullName()``). However, the :guilabel:`Info` column does not display the string returned from ``str()``, but
instead, it displays strings produced by a ``cMessagePrinter`` object. Message printers can be dynamically registered.

During Simulation
~~~~~~~~~~~~~~~~~

Qtenv sets up a network by calling ``simulation.setupNetwork()``, then immediately proceeds to invoke
``callInitialize()`` on the root module. During simulation, ``simulation.takeNextEvent()`` and
``simulation.executeEvent()`` are called iteratively. When the simulation ends, Qtenv invokes ``callFinish()`` on the
root module; the same happens when you select the :guilabel:`Conclude Simulation` menu item. The purpose of
``callFinish()`` is to record summary statistics at the end of a successful simulation run, so it will be skipped if an
error occurs during simulation. On exit, and before a new network is set up, ``simulation.deleteNetwork()`` is called.

Animation
~~~~~~~~~

To refresh the visual representation of the simulation, Qtenv regularly invokes ``simulation.callRefreshDisplay()``,
which in turn invokes the ``refreshDisplay()`` methods of modules and figures. The ``refreshDisplay()`` methods are
defined as parts of the simulation model, and they normally update display strings, and/or move, update, create or
delete figures on module canvases.

``refreshDisplay()`` is invoked after each simulation event, or with a certain frame rate when smooth animation is
enabled. The frame rate during smooth simulation is adaptive. Certain properties of the animation can be controlled via
settings in the :guilabel:`Animation Parameters` dialog. To learn more about smooth animation, see the section of
similar title in the Simulation Manual.

Extending Qtenv
~~~~~~~~~~~~~~~

It is possible for the user to contribute new inspector types without modifying Qtenv code. For this, the inspector C++
code needs to include Qtenv header files and link with the Qtenv library. One caveat is that the Qtenv headers are not
public API and thus subject to change in a new version of |omnet++|.



Command-line and Configuration Options
--------------------------------------

Command-Line Options
~~~~~~~~~~~~~~~~~~~~

A simulation program built with Qtenv accepts the following command-line switches:

- ``-h``: The program prints a help message and exits.
- ``-u Qtenv``: Causes the program to start with Qtenv (this is the default, unless the program hasn't been linked with
  Qtenv or has another custom environment library with a higher priority than Qtenv).
- ``-f filename``: Specifies the name of the configuration file. The default is ``omnetpp.ini``. Multiple ``-f``
  switches can be given; this allows partitioning your configuration file. For example, one file can contain your
  general settings, another one can contain most of the module parameters, and a third one can contain the module
  parameters you frequently change. The ``-f`` switch is optional and can be omitted.
- ``-l filename``: Loads a shared library (``.so`` file on Unix, ``.dll`` on Windows, and ``.dylib`` on Mac OS X).
  Multiple ``-l`` switches are accepted. Shared libraries may contain simple modules and other arbitrary code. File
  names may be specified without the file extension and the ``lib`` name prefix (i.e., ``foo`` instead of
  ``libfoo.so``).
- ``-n filepath``: When present, overrides the ``NEDPATH`` environment variable and sets the source locations for
  simulation NED files.
- ``-c configname``: Selects an INI configuration for execution.
- ``-r runnumber``: Takes the same effect as (but takes priority over) the :guilabel:`qtenv-default-run=` INI file
  configuration option. Run filters are also accepted. If there is more than one matching run, they are grouped at the
  top of the combobox.

Environment Variables
~~~~~~~~~~~~~~~~~~~~~

- ``OMNETPP_IMAGE_PATH``: Controls where Qtenv loads images for network graphics (modules, background, etc.) from. The
  value should be a semicolon-separated list of directories, but on non-Windows systems, the colon is also accepted as a
  separator. The default is ``./bitmaps;./images;<omnetpp>/images``, which means that Qtenv looks into the ``bitmaps``
  and ``images`` folders of the simulation, as well as the ``images`` folder in your installation's working directory.
  The directories will be scanned recursively, and subdirectory names become part of the icon name. For example, if an
  ``images/`` directory is listed, the file ``images/misc/foo.png`` will be registered as an icon named ``misc/foo``.
  Qtenv accepts PNG, JPG, and GIF files.
- ``OMNETPP_DEBUGGER_COMMAND``: When set, it overrides the factory default for the command used to launch the
  just-in-time debugger (``debugger-attach-command``). It must contain '%u' (which will be substituted with the process
  ID of the simulation), and should not contain any additional '%' characters. Since the command has to return
  immediately, on Linux and macOS, it is recommended to end it with an ampersand ('&'). The settings on the command line
  or in an ``.ini`` file take precedence over this environment variable.

Configuration Options
~~~~~~~~~~~~~~~~~~~~~

Qtenv accepts the following configuration options in the INI file.

- ``qtenv-extra-stack``: Specifies the extra amount of stack (in kilobytes) reserved for each ``activity()`` simple
  module when the simulation is run under Qtenv. This value is significantly higher than the similar one for Cmdenv
  (handling GUI events requires a large amount of stack space).
- ``qtenv-default-config``: Specifies which INI file configuration Qtenv should automatically set up after startup. If
  there is no such option, Qtenv will ask which configuration to set up.
- ``qtenv-default-run``: Specifies which run of the selected configuration Qtenv should set up after startup. If there
  is no such option, Qtenv will ask.

All other settings can be changed via the GUI and are saved into ``.qtenvrc`` files.