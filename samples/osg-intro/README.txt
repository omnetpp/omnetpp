OpenSceneGraph Intro
====================

This is one of the example simulations that demonstrate 3D visualization in
OMNeT++ using OpenSceneGraph and osgEarth.

The model demonstrates very basic use of OSG. The code simply loads a model
file using osgDB::readNodeFile(), and installs it as a scene that can be
viewed in the GUI. Models include a 3D model file in OSG's own OSGB format,
and several osgEarth .earth files. An earth file is an XML description of a
map that specifies the map source, elevation data source, annotations and
much more.

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

