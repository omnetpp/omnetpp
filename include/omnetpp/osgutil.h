//==========================================================================
//   OSGUTIL.H  -  header for
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

#ifndef __OMNETPP_OSGUTIL_H
#define __OMNETPP_OSGUTIL_H

#ifdef WITH_OSG

#include <osg/Group>

#include "ccomponent.h"
#include "csimulation.h"

namespace omnetpp {

// IMPORTANT: This class must NOT be used within the simulation kernel, so
// the sim. kernel doesn't have a linker dependency on the OSG libraries.

/**
 * osg::Node for defining correspondence to an OMNeT++ object.
 */
class SIM_API cObjectOsgNode : public osg::Group
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
    if (const cComponent *component = dynamic_cast<const cComponent*>(obj)) {
        componentId = component->getId();
        object = nullptr;
    }
    else {
        componentId = 0;
        object = obj;
    }
}

} // namespace omnetpp

#endif // WITH_OSG

#endif
