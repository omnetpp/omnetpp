//==========================================================================
//   COSGCANVAS.CC  -  part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cosgcanvas.h"

#ifdef WITH_OSG

#include "omnetpp/globals.h"
#include "omnetpp/osgutil.h" // OmnetppObjectNode
#include "osg/Node"
#include "osgEarth/Viewpoint"

namespace omnetpp {

Register_Class(cOsgCanvas);

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
    cameraManipulatorType(CAM_AUTO), fieldOfViewAngle(30), zNear(1.0), zFar(10000.0),
    genericViewpoint(new Viewpoint()),
    earthViewpoint(new osgEarth::Viewpoint())
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

std::string cOsgCanvas::info() const
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

void cOsgCanvas::setGenericViewpoint(const Viewpoint& viewpoint)
{
    *genericViewpoint = viewpoint;
}

void cOsgCanvas::setEarthViewpoint(const osgEarth::Viewpoint& viewpoint)
{
    *earthViewpoint = viewpoint;
}

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
