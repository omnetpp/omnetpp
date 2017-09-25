//==============================================================
//   SIM_STD_OSG.MSG  - part of
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


enum omnetpp::cOsgCanvas::ViewerStyle;
enum omnetpp::cOsgCanvas::CameraManipulatorType;

cplusplus
{{

namespace omnetpp {

typedef cFigure::Color Color;
typedef cOsgCanvas::Vec3d Vec3d;

Register_Enum(cOsgCanvas::ViewerStyle, (cOsgCanvas::STYLE_GENERIC, cOsgCanvas::STYLE_EARTH));
Register_Enum(cOsgCanvas::CameraManipulatorType, (cOsgCanvas::CAM_AUTO, cOsgCanvas::CAM_TRACKBALL, cOsgCanvas::CAM_EARTH));

} // namespace omnetpp

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
#if defined(__clang__)
    return "";  // FIXME clang 3.7 (3.6 is ok) has issues because of a binary ABI incompatibility the compiler cannot link with node->getDescription() - remove this hack once clang is fixed
#else
    return node->getDescription(i);
#endif
}

}}


// fully qualified forward declarations for cObjectOsgNode
class osg::Node extends void;
class osg::Group extends osg::Node;

// this must not be in any .msg namespace, because we are crossing C++ namespaces
class omnetpp::cObjectOsgNode extends osg::Group {
    // the getter with the const_cast should not be necessary (hopefully) with a future version of msgc which will static_cast to a const pointer
    cObject *object @getter(const_cast<cObject*>($->getObject())) @tostring(->str());
}


namespace omnetpp;

struct Color;
struct Vec3d;

struct cOsgCanvas::Viewpoint
{
    bool valid @editable;
    Vec3d eye @tostring(.str()); // TODO fromstr for Vec3d
    Vec3d center @tostring(.str());
    Vec3d up @tostring(.str());
}

struct cOsgCanvas::EarthViewpoint
{
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
    osg::Node *scene;
    int viewerStyle @editable @enum(omnetpp::cOsgCanvas::ViewerStyle) @group(viewer hints);
    Color clearColor @editable @tostring(.str()) @fromstring(cFigure::parseColor($)) @group(viewer hints);
    int cameraManipulatorType @editable @enum(omnetpp::cOsgCanvas::CameraManipulatorType) @group(viewer hints);
    double fieldOfViewAngle @editable @group(viewer hints);
    double zNear @editable @group(viewer hints);
    double zFar @editable @group(viewer hints);
    cOsgCanvas::Viewpoint genericViewpoint;
    cOsgCanvas::EarthViewpoint earthViewpoint;
}

namespace osg;

class Node extends void
{
    string descriptions[] @getter(osgNode_getDescription($,i)) @sizeGetter(osgNode_getNumDescriptions($));
    Node *children[] @getter(osgNode_getChild($,i)) @sizeGetter(osgNode_getNumChildren($));
}

class Group extends Node {
    // must be defined so cObjectOsgNode inherits members from Node
}
