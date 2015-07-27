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
#include "osg/Node"
#include "osgEarth/Viewpoint"

NAMESPACE_BEGIN

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
    scene(scene), viewerStyle(viewerStyle), clearColor(Color(128, 128, 220)), cameraManipulatorType(CAM_AUTO),
    fieldOfViewAngle(30), aspect(1.0), zNear(1.0), zFar(10000.0), viewpoint(new osgEarth::Viewpoint())

{
    ref(scene);
}

cOsgCanvas::~cOsgCanvas()
{
    unref(scene);
    delete viewpoint;
}

void cOsgCanvas::copy(const cOsgCanvas& other)
{
    setScene(other.getScene());

    clearColor = other.clearColor;
    cameraManipulatorType = other.cameraManipulatorType;
    fieldOfViewAngle = other.fieldOfViewAngle;
    aspect = other.aspect;
    zNear = other.zNear;
    zFar = other.zFar;
    *viewpoint = *other.viewpoint;
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

void cOsgCanvas::setEarthViewpoint(const osgEarth::Viewpoint& viewpoint)
{
    *this->viewpoint = viewpoint;
}

NAMESPACE_END

#endif // WITH_OSG
