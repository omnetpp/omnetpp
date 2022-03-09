"""
The sole purpose of this module is to keep references to the
Py4J Gateway (connecting the Python process to the IDE);
and also to some Java *Provider objects to be used by the
omnetpp.scave.{chart,results,ideplot} modules, and by our
custom matplotlib backend.
This way they can be easily accessed by all the modules that
need them, without having to worry too much about which
modules import which ones, and in which order.
"""

__copyright__ = "Copyright 2016-2020, OpenSim Ltd"
__license__ = """
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
"""

chart_provider = None  # will be set later
results_provider = None  # will be set later
widget_provider = None  # will be set later
chart_plotter = None  # will be set later
warning_annotator = None  # will be set later

gateway = None # will be set later
