//==========================================================================
//   OSGUTIL.CC  -  header for
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

#include "omnetpp/osgutil.h"
#include "omnetpp/cobject.h"
#include "omnetpp/csimulation.h"

#ifdef WITH_OSG

namespace omnetpp {

OmnetppObjectNode::OmnetppObjectNode() : componentId(0), object(nullptr)
{
}

OmnetppObjectNode::OmnetppObjectNode(cObject* object) : componentId(0), object(nullptr)
{
    setObject(object);
}

OmnetppObjectNode::OmnetppObjectNode(const OmnetppObjectNode& node, const osg::CopyOp& copyop) : Group(node, copyop)
{
    componentId = node.componentId;
    object = node.object;
}

OmnetppObjectNode::~OmnetppObjectNode()
{
}

cObject* OmnetppObjectNode::getObject() const
{
    return componentId != 0 ? getSimulation()->getComponent(componentId) : object;
}

void OmnetppObjectNode::setObject(cObject* obj)
{
    if (cComponent *component = dynamic_cast<cComponent*>(obj)) {
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
