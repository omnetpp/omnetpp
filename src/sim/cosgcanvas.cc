//==========================================================================
//   COSGCANVAS.CC  -  part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


/*
 * Implementation note: We don't want OMNeT++ to link directly with the OSG libraries
 * even when when WITH_OSG is defined, because they pull in tons of other libraries
 * while most simulations don't use 3D visualization at all. OSG support is loaded
 * dynamically by Qtenv from a shared lib when needed.
 *
 * Therefore, cOsgCanvas MUST NOT CREATE ANY LINKER REFERENCE TO THE OSG LIBS.
 * osg::Node::ref() and unref(), the use of which cannot be avoided, are accessed
 * via cEnvir::refOsgNode() and unrefOsgNode() that delegate to the dynamically
 * loaded OSG support library.
 */

#include <cmath>
#include <limits>
#include "omnetpp/globals.h"
#include "omnetpp/csimulation.h"  // getEnvir()
#include "omnetpp/cosgcanvas.h"

namespace omnetpp {

Register_Class(cOsgCanvas);

static double NaN = std::numeric_limits<double>::quiet_NaN();

inline void ref(osg::Node *scene)
{
    if (scene)
        getEnvir()->refOsgNode(scene);
}

inline void unref(osg::Node *scene)
{
    if (scene)
        getEnvir()->unrefOsgNode(scene);
}

cOsgCanvas::cOsgCanvas(const char *name, ViewerStyle viewerStyle, osg::Node *scene) : cOwnedObject(name),
    scene(scene), viewerStyle(viewerStyle), zNear(NaN), zFar(NaN),
    genericViewpoint(new Viewpoint()), earthViewpoint(new EarthViewpoint())
{
    ref(scene);
}

cOsgCanvas::~cOsgCanvas()
{
    unref(scene);
    delete genericViewpoint;
    delete earthViewpoint;
}

void cOsgCanvas::copy(const cOsgCanvas& other)
{
    setScene(other.getScene());

    clearColor = other.clearColor;
    cameraManipulatorType = other.cameraManipulatorType;
    fieldOfViewAngle = other.fieldOfViewAngle;
    zNear = other.zNear;
    zFar = other.zFar;
    *earthViewpoint = *other.earthViewpoint;
}

cOsgCanvas& cOsgCanvas::operator=(const cOsgCanvas& other)
{
    if (this == &other)
        return *this;
    cOwnedObject::operator=(other);
    copy(other);
    return *this;
}

std::string cOsgCanvas::str() const
{
    if (scene == nullptr)
        return "(empty)";
    return "";
}

void cOsgCanvas::setScene(osg::Node *scene)
{
    ref(scene);
    unref(this->scene);
    this->scene = scene;
}

void cOsgCanvas::clearZLimits()
{
    zNear = zFar = NaN;
}

bool cOsgCanvas::hasZLimits() const
{
    return !std::isnan(zNear) && !std::isnan(zFar);
}

void cOsgCanvas::setGenericViewpoint(const Viewpoint& viewpoint)
{
    *genericViewpoint = viewpoint;
}

void cOsgCanvas::setEarthViewpoint(const EarthViewpoint& viewpoint)
{
    *earthViewpoint = viewpoint;
}

std::string cOsgCanvas::Vec3d::str()
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
}

}  // namespace omnetpp
