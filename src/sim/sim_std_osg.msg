
//==============================================================
//   SIM_STD_OSG.MSG  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==============================================================

//--------------------------------------------------------------
// Copyright (C) 2015 OpenSim Ltd.
//
//  This file is distributed WITHOUT ANY WARRANTY. See the file
//  `license' for details on this and other legal matters.
//--------------------------------------------------------------

enum omnetpp::cOsgCanvas::ViewerStyle;
enum omnetpp::cOsgCanvas::CameraManipulatorType;

cplusplus
{{
NAMESPACE_BEGIN

typedef cFigure::Color Color;

Register_Enum(cOsgCanvas::ViewerStyle, (cOsgCanvas::STYLE_GENERIC, cOsgCanvas::STYLE_EARTH));
Register_Enum(cOsgCanvas::CameraManipulatorType, (cOsgCanvas::CAM_AUTO, cOsgCanvas::CAM_TRACKBALL, cOsgCanvas::CAM_EARTH));

cFigure::Color parseColor(const char *s);

NAMESPACE_END

#ifndef USE_NAMESPACE
#define omnetpp   //XXX horrible hack to make it compile with the omnetpp namespace turned off
#endif

#include <osg/Node>
#include <osg/Group>
//TODO #include <osgEarth/Viewpoint>

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
#if defined(_WIN32) && defined(__clang__)
    return "";  // FIXME as of clang 3.7 on Windows (MinGW64) because of a binary ABI incompatibility the compiler cannot link with node->getDescription() - remove this hack once clang is fixed
#else
    return node->getDescription(i);
#endif
}

//TODO namespace osgEarth {
//TODO     inline std::ostream& operator<<(std::ostream& os, const Viewpoint& viewpoint) {
//TODO         return os << viewpoint.toString();
//TODO     }
//TODO } // namespace

}}

namespace omnetpp;  //XXX this ought to be conditional on USE_NAMESPACE but cannot; see hack above!

class osg::Node extends void;
class osgEarth::Viewpoint extends void;
class cOsgCanvas extends cOwnedObject;
struct Color;

class cOsgCanvas extends cOwnedObject
{
    osg::Node *scene;
    int viewerStyle @editable @enum(omnetpp::cOsgCanvas::ViewerStyle) @group(viewer hints);
    Color clearColor @editable @tostring(.str()) @fromstring(parseColor($)) @group(viewer hints);
    int cameraManipulatorType @editable @enum(omnetpp::cOsgCanvas::CameraManipulatorType) @group(viewer hints);
    double fieldOfViewAngle @editable @group(viewer hints);
    double zNear @editable @group(viewer hints);
    double zFar @editable @group(viewer hints);
//TODO    osgEarth::Viewpoint earthViewpoint @group(viewer hints) @tostring(.toString());
}
namespace osg;

class Node extends void
{
    //string className @getter(className);
    string descriptions[] @getter(osgNode_getDescription($,i)) @sizeGetter(osgNode_getNumDescriptions($));
    Node *children[] @getter(osgNode_getChild($,i)) @sizeGetter(osgNode_getNumChildren($));
}


//TODO
//namespace osgEarth;
//
//class Viewpoint extends void
//{
//    string name;
//    bool isValid @getter(isValid);
//    double x @getter(x);
//    double y @getter(y);
//    double z @getter(z);
//    double heading @editable;
//    double pitch @editable;
//    double range @editable;
//    //TODO string SRS; //const osgEarth::SpatialReference *getSRS()
//}
