//==========================================================================
//  COMPONENTHISTORY.CC - part of
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

#include "omnetpp/cmodule.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/csimplemodule.h"
#include "componenthistory.h"

namespace omnetpp {
namespace qtenv {

void ComponentHistory::componentDeleted(cComponent *component)
{
    Data& data = components[component->getId()];
    cModule *parent = component->getParentModule();
    data.parentModuleId = parent ? parent->getId() : -1;
    data.fullName = component->getFullName();
    data.componentType = component->getComponentType();
}

void ComponentHistory::componentReparented(cComponent *component, cModule *oldParent, int oldId)
{
    // see cModule::changeParentTo() for explanation
    Data& data = components[oldId];
    data.parentModuleId = oldParent ? oldParent->getId() : -1;
    data.fullName = component->getFullName();
    data.componentType = component->getComponentType();
}

int ComponentHistory::getParentModuleId(int componentId) const
{
    cComponent *component = getSimulation()->getComponent(componentId);
    if (component) {
        cModule *parent = component->getParentModule();
        return parent ? parent->getId() : -1;
    }
    else {
        const Data& data = lookup(componentId);
        return data.parentModuleId;
    }
}

const char *ComponentHistory::getComponentFullName(int componentId) const
{
    cComponent *component = getSimulation()->getComponent(componentId);
    if (component) {
        return component->getFullName();
    }
    else {
        const Data& data = lookup(componentId);
        return data.fullName.c_str();
    }
}

std::string ComponentHistory::getComponentFullPath(int componentId) const
{
    cComponent *component = getSimulation()->getComponent(componentId);
    if (component) {
        return component->getFullPath();
    }
    else {
        const Data& data = lookup(componentId);
        return data.parentModuleId == -1 ? data.fullName.c_str() : getComponentFullPath(data.parentModuleId) + "." + data.fullName.c_str();
    }
}

std::string ComponentHistory::getComponentRelativePath(int componentId, int rootComponentId) const
{
    // returned path does not include the name of the specified root component
    if (componentId == rootComponentId)
        return "<parent>";

    cComponent *component = getSimulation()->getComponent(componentId);
    if (component) {
        cModule *parent = component->getParentModule();
        return (parent == nullptr || parent->getId() == rootComponentId) ? component->getFullName() :
               getComponentRelativePath(parent->getId(), rootComponentId) + "." + component->getFullName();
    }
    else {
        const Data& data = lookup(componentId);
        return (data.parentModuleId == -1 || data.parentModuleId == rootComponentId) ? data.fullName.c_str() :
               getComponentRelativePath(data.parentModuleId, rootComponentId) + "." + data.fullName.c_str();
    }
}

cComponentType *ComponentHistory::getComponentType(int componentId) const
{
    cComponent *component = getSimulation()->getComponent(componentId);
    if (component) {
        return component->getComponentType();
    }
    else {
        const Data& data = lookup(componentId);
        return data.componentType;
    }
}

std::string ComponentHistory::getComponentTypeName(int componentId) const
{
    return getComponentType(componentId)->getFullPath();
}

void ComponentHistory::clear()
{
    components.clear();
}

}  // namespace qtenv
}  // namespace omnetpp

