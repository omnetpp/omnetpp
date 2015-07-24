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

NAMESPACE_BEGIN

Register_Class(cOsgCanvas);
Register_Class(cOsgEarthCanvas);


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

cOsgCanvas::cOsgCanvas(const char *name, osg::Node *scene) : cOwnedObject(name),
    scene(scene), clearColor(Color(128, 128, 220)), cameraManipulatorType(CAM_TRACKBALL),
    fieldOfViewAngle(30), aspect(1.0), zNear(1.0), zFar(10000.0)
{
    ref(scene);
}

cOsgCanvas::~cOsgCanvas()
{
    unref(scene);
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

//----

cOsgEarthCanvas::cOsgEarthCanvas(const char *name, osg::Node *scene) :
    cOsgCanvas(name, scene)
{
    cameraManipulatorType = CAM_EARTH;
    clearColor = Color(0, 0, 80);
}

cOsgEarthCanvas& cOsgEarthCanvas::operator=(const cOsgEarthCanvas& other)
{
    if (this == &other)
        return *this;
    cOsgCanvas::operator=(other);
    copy(other);
    return *this;
}

std::string cOsgEarthCanvas::info() const
{
    return cOsgCanvas::info(); // TODO
}

NAMESPACE_END

#endif // WITH_OSG
