//==========================================================================
//   OSGUTIL.H  -  header for
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

#ifndef __OMNETPP_OSGUTIL_H
#define __OMNETPP_OSGUTIL_H

#include <omnetpp.h>

#ifdef WITH_OSG

#include <osg/Group>

namespace omnetpp {

// IMPORTANT: This class must NOT be used within the simulation kernel, so
// the sim. kernel doesn't have a linker dependency on the OSG libraries.

/**
 * @brief An osg::Group for defining correspondence of a 3D object to an
 * \opp object.
 *
 * Using this node in an OSG scene graph associates its children (and all
 * descendants) with a simulation object. The effect is that when a such
 * a node is clicked in the OSG viewer, the corresponding simulation object
 * can be inspected in the runtime GUI.
 *
 * Example usage:
 *
 * ```
 * cModule *carModule = ...
 * osg::Node *carModel = ...
 * osg::Group *parent = ...
 * cObjectOsgNode *wrapperNode = new cObjectOsgNode();
 * wrapperNode->setObject(carModule);
 * wrapperNode->addChild(carModel);
 * parent->addChild(wrapperNode);
 * ```
 *
 * The \opp object should exist as long as the wrapper node exists. Otherwise,
 * clicking child nodes with the mouse is likely to result in a crash.
 *
 * @ingroup OSG
 */
class cObjectOsgNode : public osg::Group
{
    protected:
        int componentId; // 0=none
        const cObject *object; // object pointer; if componentId!=0, it takes precedence

    protected:
        virtual ~cObjectOsgNode() {}

    public:
        cObjectOsgNode() : componentId(0), object(nullptr) {}
        cObjectOsgNode(const cObject *object) : componentId(0), object(nullptr) { setObject(object);}
        cObjectOsgNode(const cObjectOsgNode& node, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
        META_Node(osg, cObjectOsgNode);

        const cObject *getObject() const;
        void setObject(const cObject *obj);
};

inline cObjectOsgNode::cObjectOsgNode(const cObjectOsgNode& node, const osg::CopyOp& copyop) : Group(node, copyop)
{
    componentId = node.componentId;
    object = node.object;
}

inline const cObject *cObjectOsgNode::getObject() const
{
    return componentId != 0 ? getSimulation()->getComponent(componentId) : object;
}

inline void cObjectOsgNode::setObject(const cObject *obj)
{
    // if the object is a component, store ID instead so we can avoid having
    // a dangling pointer when it's deleted
    if (const cComponent *component = dynamic_cast<const cComponent*>(obj)) {
        componentId = component->getId();
        object = nullptr;
    }
    else {
        componentId = 0;
        object = obj;
    }
}

}  // namespace omnetpp

#endif // WITH_OSG

#endif

