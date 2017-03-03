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

#include "omnetpp/cosgcanvas.h"

#ifdef WITH_OSG

#include <cmath>
#include <limits>
#include "omnetpp/globals.h"
#include "omnetpp/osgutil.h" // OmnetppObjectNode
#include "osg/Node"
#ifdef WITH_OSGEARTH
#include "osgEarth/Viewpoint"
#endif

namespace omnetpp {

Register_Class(cOsgCanvas);

static double NaN = std::numeric_limits<double>::quiet_NaN();

inline void ref(osg::Node *scene)
{
    if (scene)
        scene->ref();
}

inline void unref(osg::Node *scene)
{
    if (scene)
        scene->unref();
}

cOsgCanvas::cOsgCanvas(const char *name, ViewerStyle viewerStyle, osg::Node *scene) : cOwnedObject(name),
    scene(scene), viewerStyle(viewerStyle), clearColor(Color(128, 128, 220)),
    cameraManipulatorType(CAM_AUTO), fieldOfViewAngle(30), zNear(NaN), zFar(NaN),
    genericViewpoint(new Viewpoint())
#ifdef WITH_OSGEARTH
    ,earthViewpoint(new osgEarth::Viewpoint())
#endif
{
    ref(scene);
}

cOsgCanvas::~cOsgCanvas()
{
    unref(scene);
    delete genericViewpoint;
#ifdef WITH_OSGEARTH
    delete earthViewpoint;
#endif
}

void cOsgCanvas::copy(const cOsgCanvas& other)
{
    setScene(other.getScene());

    clearColor = other.clearColor;
    cameraManipulatorType = other.cameraManipulatorType;
    fieldOfViewAngle = other.fieldOfViewAngle;
    zNear = other.zNear;
    zFar = other.zFar;
#ifdef WITH_OSGEARTH
    *earthViewpoint = *other.earthViewpoint;
#endif
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
    return std::string("scene '") + scene->getName() + "'";
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

#ifdef WITH_OSGEARTH
void cOsgCanvas::setEarthViewpoint(const osgEarth::Viewpoint& viewpoint)
{
    *earthViewpoint = viewpoint;
}
#endif

cOsgCanvas::Vec3d::operator osg::Vec3d() const {
    return osg::Vec3d(x, y, z);
}

} // namespace omnetpp

#else

// Dummy cOsgCanvas class in case OpenSceneGraph is not available
namespace omnetpp {

cOsgCanvas::cOsgCanvas() : cOwnedObject() { }

}

#endif // WITH_OSG
