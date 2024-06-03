Extending the IDE
=================

There are several ways to extend the functionality of the |omnet++| IDE. The Simulation IDE is based on the Eclipse
platform but extends it with new editors, views, wizards, and other functionality.

Installing New Features
-----------------------

Because the IDE is based on the Eclipse platform, it is possible to add additional features that are available for
Eclipse. The installation procedure is exactly the same as with a standard Eclipse distribution. Choose the
:menuselection:`Help --> Install New Software` menu item and select an existing Update Site to work with or add a new Site
(using the site URL) to the Available Software Sites. After the selection, you can browse and install the packages the
site offers.

To learn about installing new software into your IDE, please visit the :guilabel:`Updating and installing software` topic
in the :guilabel:`Workbench User Guide`. You can find the online help system in the :menuselection:`Help --> Help Contents`
menu.

.. tip::

   There are thousands of useful components and extensions for Eclipse. The best places to start looking for extensions
   are the Eclipse Marketplace (http://marketplace.eclipse.org/) and the Eclipse Plugins info site
   (http://www.eclipse-plugins.info).

Adding New Wizards
------------------

The Simulation IDE makes it possible to contribute new wizards to the wizard dialogs under the :menuselection:`File --> New`
menu without writing Java code or requiring any knowledge of Eclipse internals. Wizards can create new simulation
projects, new simulations, new NED files, or other files by using templates or perform export/import functions. Wizard
code is placed under the :file:`templates` folder of an |omnet++| project, which makes it easy to distribute wizards
with the model. When the user imports and opens a project that contains wizards, the wizards will automatically become
available.

.. tip::

   The way to create wizards is documented in the :ref:`ide-customization-guide`.

Project-Specific Extensions
---------------------------

It is possible to install an Eclipse plug-in by creating a :file:`plugins` folder in an |omnet++| project and copying
the plug-in JAR file to that folder (this mechanism is implemented as part of the Simulation IDE and does not work in
generic Eclipse installations or with non-|omnet++| projects). This extension mechanism allows the distribution of
model-specific IDE extensions together with a simulation project without requiring the end user to do extra deployment
steps to install the plug-in. Plugins and wizards that are distributed with a project are automatically activated when
the host project is opened.

Eclipse plug-in JAR files can be created using the `Plug-in Development Environment <https://eclipse.org/pde>`_. The
|omnet++| IDE does not contain the PDE by default; however, it can be easily installed if necessary.

.. tip::

   Read the :ref:`ide-developers-guide` for more information on how to install the PDE and how to
   develop plug-in extensions for the IDE.

