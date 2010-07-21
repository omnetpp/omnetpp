Google Earth Demo
=================

This is a demo for visualizing a simulation using Google Earth.

The demo simulates a wireless mobile ad-hoc network. The model contains a
number of mobile nodes (cars) that move about randomly over a 2km-by-2km
area. The nodes have identical radios, with a transmission range of about
500m; for simplicity, we assume that the area covered by each radio is
a perfect circle. When two modes move within range of each other, they can
communicate with each other, forming a (usually disconnected) ad-hoc
network. The purpose of the simulation study could be to measure how long
it takes to disseminate some piece of information to all nodes of the
network. The communication itself is not modeled in this demo.

Visualization shows the mobile nodes themselves (a 3D car model), their
recent trails, their transmission ranges (semi-transparent filled circles
on the terrain), and the current connectivity of the network (cars that are
closer than 500m are connected with a white line).

How to view:

First, start the simulation in Fast or Express mode. Then, if you are on a
platform where the browser supports the Google Earth plugin (this currently
means Windows or OS X, sadly there is no GE browser plugin for Linux at the
time of writing), start the browser and point it to http://localhost:4242.
The browser should display a page with the Google Earth plugin on it, and
the view will fly to the playground. Then you can move the camera freely
with the mouse (e.g. use shift+drag to view the site from multiple angles).

Alternatively, install the standalone Google Earth program (this is the
only possibility on Linux, as there is no browser plugin), and open the
provided Demo.kml in it; it will add a new placemark to the tree. You may
need to turn on the placemark manually. Demo.kml refers to KML dynamically
produced by the simulation, so the simulation should be already running (in
Fast or Express mode). If you don't see the animation, refresh the
placemark using its context menu.

How it works:

Demo.kml adds a network link to Google Earth; the link points to
http://localhost:4242/snapshot.kml, and is configured to reload
periodically. The index.html used in the browser version basically does the
same, using some Javascript code. The URL is served by the simulation, and
the returned KML document contains the visualization of the current state
of the simulation. The simulation runs in real-time, with socket listening
integrated into the real-time scheduler. (This is just an artifact of
having used the "Sockets" sample simulation as the starting point of this
demo; it would be possible, moreover advisable, to change the code to have
a dedicated HTTP server that runs in a different thread, completely
independent of the scheduler.)

The HTTP GET request is processed by the KmlHttpServer module, which has
one instance in the simulation model. The module assembles the KML file
from pieces, where each piece is contributed by a IKmlFragmentProvider
object. All mobile nodes (MobileNode modules) and the ChannelController
module are registered as IKmlFragmentProviders at KmlHttpServer; the former
ones produce KML for the visualizations of the mobile nodes they represent
(model, transmission range, trail), and the latter provides KML for the
connectivity graph. To keep the source code of MobileNode and
ChannelController concise, code to produce actual KML markup has been
factored out to utility methods of a KmlUtil class. Translation of metric
playground (x,y) coordinates to geographical latitude/longitude coordinates
is also in KmlUtil.

Future work:

For production code, we'd probably decouple model from visualization,
i.e. split MobileNode into a module that deals with mobility and into
a separate one that listens to position changes and generates KML from
them. Also, serving KML via a built-in dedicated web server (e.g. an
embedded Mongoose instance) would mean that any simulation scheduler
(sequential, real-time, parsim, etc.) can be used without change.

