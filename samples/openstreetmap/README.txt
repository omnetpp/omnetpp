OpenStreetMap
=============

This example simulation demonstrates several OMNeT++ features, from using
XML files as input to using the canvas for graphics rendering and animation.

The simulation displays a city map, with some animated car traffic on it.
Map data comes from an OSM file exported from openstreetmap.org.
OSM files are XML files with quite a simple structure: <node> elements
represent special points in the map (endpoints and breakpoints of streets,
corners of buildings, points of interest, etc.), and <way> elements group
nodes into more complex items such as building and streets. (<relation>
elements, the third main element type, are ignored in this demo.)
The model obtains the map file via an XML NED parameter, and reads it
into a data structure that directly mirrors file contents.
(To keep the code simple, this example uses plain structs for the data
structure. Note that a more sophisticated OSM data structure can be found
in newer versions of INET Framework.)

Map rendering is done using the Canvas API, by placing polygon and polyline
items on the canvas. Polygons are used for buildings, greens and similar
areas, while roads and streets are simply polylines with a width greater
than one, and a color of white or yellow. Names are set on the items as
tooltips.

Cars are visualized on the map as color rectangles. The number of cars
can be specified in omnetpp.ini, and they are initially placed
at random positions (at random map nodes, actually). A car is composed
of two collaborating submodules, a "driver" and "route planner".
Quite intuitively, the "driver" model is supposed to drive the car along
the route supplied by "route planner". Naturally, one can imagine
arbitrarily sophisticated (and therefore, complicated) behavior for
both components. However, the implementation present in this model is
intentionally kept simple to facilitate understanding. The "driver"
simply moves the car at a constant but random speed towards the next node.
When the node is reached, the "driver" asks the "route planner"
for a new target node. The "route planner" choses the next target
randomly from the neighbours of the current node. No attempt is made
for planning realistic or meaningful routes. The driver model
does not account for finite acceleration/deceleration or smooth
turns, and since it completely disregards other cars, overlaps
can also occur.

It can be interesting how animation is implemented. The "driver"
module only schedules simulation events when the car reaches a node
and a new target node is selected. Position updates are done entirely
from the refreshDisplay() method, which the GUI (Qtenv) invokes as
often as it needs to (depending on the speed and frame rate of the
animation). This method allows smooth animation without the need
for scheduling frequent periodic simulation events for position
update.

---

OpenStreetMap (http://www.openstreetmap.org) is a collaborative project
to create a free editable map of the world.

This example simulation requires the Qtenv runtime environment.

