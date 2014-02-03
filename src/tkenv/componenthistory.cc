//==========================================================================
//  COMPONENTHISTORY.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cmodule.h"
#include "cchannel.h"
#include "csimplemodule.h"
#include "componenthistory.h"

NAMESPACE_BEGIN

void ComponentHistory::componentDeleted(cComponent *component)
{
    if (!component->isModule()) return;  // TODO remove this in 5.0
    Data& data = components[((cModule*)component)->getId()]; //TODO remove cast in 5.0
    cModule *parent = component->getParentModule();
    data.parentModuleId = parent ? parent->getId() : -1;
    data.fullName = component->getFullName();
    data.componentType = component->getComponentType();
}

/*TODO add back in 5.0
void ComponentHistory::componentReparented(cComponent *component, cModule *oldParent, int oldId)
{
    // see cModule::changeParentTo() for explanation
    Data& data = components[oldId];
    data.parentModuleId = oldParent ? oldParent->getId() : -1;
    data.fullName = component->getFullName();
    data.componentType = component->getComponentType();
}
*/

int ComponentHistory::getParentModuleId(int componentId) const
{
    cComponent *component = simulation.getModule(componentId); //TODO change to getComponent in 5.0
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
    cComponent *component = simulation.getModule(componentId); //TODO change to getComponent in 5.0
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
    cComponent *component = simulation.getModule(componentId); //TODO change to getComponent in 5.0
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

    cComponent *component = simulation.getModule(componentId); //TODO change to getComponent in 5.0
    if (component) {
        cModule *parent = component->getParentModule();
        return (parent == NULL || parent->getId()==rootComponentId) ? component->getFullName() :
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
    cComponent *component = simulation.getModule(componentId); //TODO change to getComponent in 5.0
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

NAMESPACE_END

