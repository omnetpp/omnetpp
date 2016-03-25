OSG Satellites Demo
===================

This is one of the example simulations that demonstrate 3D visualization in
OMNeT++ using OpenSceneGraph and osgEarth.

The model demonstrates visualizing satellites orbiting the Earth. The model
uses the osgEarth library, thus, the Earth 3D model is loaded from an
.earth file. Satellites and their ground stations (large antenna dishes)
are loaded from .osgb files. Satellite orbits are parameterized via module
parameters in omnetpp.ini. Several scenarios are configured, including
random, geostationary and polar orbits.

---

OpenSceneGraph (http://www.openscenegraph.org), or OSG for short, is an
open source high-performance 3D graphics toolkit, used by application
developers in fields such as visual simulation, games, virtual reality,
scientific visualization and modeling.

osgEarth (http://osgearth.org/) is a terrain rendering library implemented
on top of OpenSceneGraph. osgEarth can work with many data sources
including OpenStreetmap, Yahoo Maps, Bing Maps, MapQuest and ReadyMap, just
to name a few.

This example simulation requires the Qtenv runtime environment.

