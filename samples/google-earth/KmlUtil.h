//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __KMLUTIL_H__
#define __KMLUTIL_H__

#include <omnetpp.h>

#ifndef M_PI
#define M_PI 3.14159265  // needed on Windows
#endif

/**
 * A collection of utility functions to generate KML markup for this demo
 */
class KmlUtil
{
  public:
    struct Pt2D {float lon, lat;  Pt2D() {lon=lat=0;}  Pt2D(float lo, float la) {lon=lo; lat=la;}};
    struct Pt3D {float lon, lat, alt;  Pt3D() {lon=lat=alt=0;}  Pt3D(float lo, float la, float al) {lon=lo; lat=la; alt=al;}};

    /** Generates the start of a KML folder; just add content and a close tag. */
    static std::string folderHeader(const char *id=NULL, const char *name=NULL, const char *description=NULL);

    /** Generates a floating placemark, displayed as a white arrow pointing downwards, with a label */
    static std::string placemark(const char *id, float lon, float lat, float alt, const char *name=NULL, const char *description=NULL);

    /** Generates placemark containing a line of connected line segments */
    static std::string lineString(const char *id, const std::vector<Pt2D>& pts, const char *name=NULL, const char *description=NULL, const char *color=NULL);

    /** Generates placemark containing several disjoint line segments */
    static std::string lines(const char *id, const std::vector<Pt2D>& pts, const char *name=NULL, const char *description=NULL, const char *color=NULL);

    /** Generates placemark containing a disc with the given radius (meters), approximated with a polygon */
    static std::string disk(const char *id, float lon, float lat, float r, const char *name=NULL, const char *description=NULL, const char *color=NULL);

    /** Generates placemark containing a 3-D model */
    static std::string model(const char *id, float lon, float lat, float heading, float scale, const char *link, const char *name=NULL, const char *description=NULL);

    /** Generates placemark containing a track (gx:Track KML element) */
    static std::string track(const char *id, const std::vector<Pt2D>& pts, float timeStep, float modelScale, const char *modelLink, const char *name=NULL, const char *description=NULL, const char *color=NULL);

    /**
     * Returns the latitude yoff meters to the north (or south if yoff<0) from latitude lat
     */
    static double y2lat(double lat, double yoff) { return lat - yoff / 111111; }

    /**
     * Returns the longitude of the place xoff meters to the east (or west if xoff<0) of the place (lon, lat)
     */
    static double x2lon(double lat, double lon, double xoff) { return lon +  xoff / 111111 / cos(fabs(lat/180*M_PI)); }

    /**
     * Converts a color given in HSB to RGB.
     */
    static void hsbToRgb(double hue, double saturation, double brightness, double& red, double& green, double &blue);
};

#endif

