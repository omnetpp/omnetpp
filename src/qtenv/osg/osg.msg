//==============================================================
//   OSG.MSG  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==============================================================

//--------------------------------------------------------------
// Copyright (C) 2017 OpenSim Ltd.
//
//  This file is distributed WITHOUT ANY WARRANTY. See the file
//  `license' for details on this and other legal matters.
//--------------------------------------------------------------

cplusplus
{{

namespace omnetpp {
Register_Enum(cOsgCanvas::ViewerStyle, (cOsgCanvas::STYLE_GENERIC, cOsgCanvas::STYLE_EARTH));
Register_Enum(cOsgCanvas::CameraManipulatorType, (cOsgCanvas::CAM_AUTO, cOsgCanvas::CAM_TRACKBALL, cOsgCanvas::CAM_EARTH));
} // namespace omnetpp

#include "qtenv/qtenvdefs.h"
#include "omnetpp/osgutil.h"
#include <osg/Node>
#include <osg/Group>

namespace osg {
    inline std::ostream& operator<<(std::ostream& os, Node *node) {
        return os << "(" << node->className() << ")";
    }
} // namespace

inline int osgNode_getNumChildren(osg::Node *node)
{
    osg::Group *group = node->asGroup();
    return group ? group->getNumChildren() : 0;
}

inline osg::Node *osgNode_getChild(osg::Node *node, unsigned int i)
{
    return node->asGroup()->getChild(i);
}

inline int osgNode_getNumDescriptions(osg::Node *node)
{
    return node->getNumDescriptions();
}

inline std::string osgNode_getDescription(osg::Node *node, unsigned int i)
{
    return node->getDescription(i);
}

}}

namespace omnetpp;

enum cOsgCanvas::ViewerStyle;
enum cOsgCanvas::CameraManipulatorType;

struct cOsgCanvas::Vec3d
{
    @existingClass;
    @descriptor(false);
    @opaque;
    @toString(.str());
    @toValue(.str());
    // TODO fromstr for Vec3d
}

struct cOsgCanvas::Viewpoint
{
    @existingClass;
    bool valid @editable;
    cOsgCanvas::Vec3d eye;
    cOsgCanvas::Vec3d center;
    cOsgCanvas::Vec3d up;
}

struct cOsgCanvas::EarthViewpoint
{
    @existingClass;
    bool valid @editable;
    double longitude @editable; // TODO grouping?
    double latitude @editable;
    double altitude @editable;
    double heading @editable;
    double pitch @editable;
    double range @editable;
}

class cOsgCanvas extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition; // in sim_std.msg
    osg::Node *scene;
    int viewerStyle @editable @enum(omnetpp::cOsgCanvas::ViewerStyle) @group(viewer hints);
    cFigure::Color clearColor @editable @group(viewer hints);
    int cameraManipulatorType @editable @enum(omnetpp::cOsgCanvas::CameraManipulatorType) @group(viewer hints);
    double fieldOfViewAngle @editable @group(viewer hints);
    double zNear @editable @group(viewer hints);
    double zFar @editable @group(viewer hints);
    cOsgCanvas::Viewpoint genericViewpoint @toValue(omnetpp::toAnyPtr(&$));
    cOsgCanvas::EarthViewpoint earthViewpoint @toValue(omnetpp::toAnyPtr(&$));
}

class cObjectOsgNode extends osg::Group
{
    @existingClass;
    // the getter with the const_cast should not be necessary (hopefully) with a future version of msgc which will static_cast to a const pointer
    cObject *object @getter(const_cast<cObject*>($->getObject())) @toString(->str());
}

namespace osg;

class Node
{
    @existingClass;
    @descriptor(readonly);
    string descriptions[] @getter(osgNode_getDescription($,i)) @sizeGetter(osgNode_getNumDescriptions($));
    Node *children[] @getter(osgNode_getChild($,i)) @sizeGetter(osgNode_getNumChildren($));
}

class Group extends Node {
    // must be defined so cObjectOsgNode inherits members from Node
    @existingClass;
}

