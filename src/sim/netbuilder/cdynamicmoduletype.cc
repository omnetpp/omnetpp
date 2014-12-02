//==========================================================================
//   CDYNAMICMODULETYPE.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#include "cdynamicmoduletype.h"
#include "cneddeclaration.h"
#include "cnedloader.h"
#include "cnednetworkbuilder.h"

NAMESPACE_BEGIN


cDynamicModuleType::cDynamicModuleType(const char *name) : cModuleType(name)
{
}

std::string cDynamicModuleType::info() const
{
    return getDecl()->info();
}

std::string cDynamicModuleType::detailedInfo() const
{
    return getDecl()->nedSource();
}

cNEDDeclaration *cDynamicModuleType::getDecl() const
{
    // do not store the pointer, because the declaration object may have been
    // thrown out of cNEDLoader to conserve memory
    cNEDDeclaration *decl = cNEDLoader::getInstance()->getDecl(getFullName());
    ASSERT(decl->getType()==cNEDDeclaration::SIMPLE_MODULE || decl->getType()==cNEDDeclaration::COMPOUND_MODULE);
    return decl;
}

bool cDynamicModuleType::isNetwork() const
{
    return getDecl()->isNetwork();
}

bool cDynamicModuleType::isSimple() const
{
    return getDecl()->getType()==cNEDDeclaration::SIMPLE_MODULE;
}

cModule *cDynamicModuleType::createModuleObject()
{
    const char *classname = getDecl()->getImplementationClassName();
    ASSERT(classname!=NULL);
    return instantiateModuleClass(classname);
}

void cDynamicModuleType::addParametersAndGatesTo(cModule *module)
{
    cNEDDeclaration *decl = getDecl();
    cNEDNetworkBuilder().addParametersAndGatesTo(module, decl);
}

void cDynamicModuleType::applyPatternAssignments(cComponent *component)
{
    cNEDNetworkBuilder().assignParametersFromPatterns(component);
}

void cDynamicModuleType::setupGateVectors(cModule *module)
{
    cNEDDeclaration *decl = getDecl();
    cNEDNetworkBuilder().setupGateVectors(module, decl);
}

void cDynamicModuleType::buildInside(cModule *module)
{
    cNEDDeclaration *decl = getDecl();
    cNEDNetworkBuilder().buildInside(module, decl);
}

cProperties *cDynamicModuleType::getProperties() const
{
    cNEDDeclaration *decl = getDecl();
    return decl->getProperties();
}

cProperties *cDynamicModuleType::getParamProperties(const char *paramName) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->getParamProperties(paramName);
}

cProperties *cDynamicModuleType::getGateProperties(const char *gateName) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->getGateProperties(gateName);
}

cProperties *cDynamicModuleType::getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->getSubmoduleProperties(submoduleName, submoduleType);
}

cProperties *cDynamicModuleType::getConnectionProperties(int connectionId, const char *channelType) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->getConnectionProperties(connectionId, channelType);
}

std::string cDynamicModuleType::getPackageProperty(const char *name) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->getPackageProperty(name);
}

const char *cDynamicModuleType::getImplementationClassName() const
{
    cNEDDeclaration *decl = getDecl();
    return decl->getImplementationClassName();
}

std::string cDynamicModuleType::getCxxNamespace() const
{
    cNEDDeclaration *decl = getDecl();
    return decl->getCxxNamespace();
}

bool cDynamicModuleType::isInnerType() const
{
    cNEDDeclaration *decl = getDecl();
    return decl->isInnerType();
}

NAMESPACE_END

