//==========================================================================
//   CDYNAMICMODULETYPE.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

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
#include "ccompoundmodule.h"

USING_NAMESPACE


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
    cNEDDeclaration *decl = cNEDLoader::instance()->getDecl(fullName());
    ASSERT(decl->getType()==cNEDDeclaration::SIMPLE_MODULE || decl->getType()==cNEDDeclaration::COMPOUND_MODULE);
    return decl;
}

bool cDynamicModuleType::isNetwork() const
{
    return getDecl()->isNetwork();
}

cModule *cDynamicModuleType::createModuleObject()
{
    const char *classname = getDecl()->implementationClassName();
    if (classname)
        return instantiateModuleClass(classname);
    else
        return new cCompoundModule();
}

void cDynamicModuleType::addParametersTo(cModule *module)
{
    cNEDDeclaration *decl = getDecl();
    cNEDNetworkBuilder().addParametersTo(module, decl);
}

void cDynamicModuleType::addGatesTo(cModule *module)
{
    cNEDDeclaration *decl = getDecl();
    cNEDNetworkBuilder().addGatesTo(module, decl);
}

void cDynamicModuleType::buildInside(cModule *module)
{
    cNEDDeclaration *decl = getDecl();
    cNEDNetworkBuilder().buildInside(module, decl);
}

cProperties *cDynamicModuleType::properties() const
{
    cNEDDeclaration *decl = getDecl();
    return decl->properties();
}

cProperties *cDynamicModuleType::paramProperties(const char *paramName) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->paramProperties(paramName);
}

cProperties *cDynamicModuleType::gateProperties(const char *gateName) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->gateProperties(gateName);
}

cProperties *cDynamicModuleType::submoduleProperties(const char *submoduleName, const char *submoduleType) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->submoduleProperties(submoduleName, submoduleType);
}

cProperties *cDynamicModuleType::connectionProperties(int connectionId, const char *channelType) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->connectionProperties(connectionId, channelType);
}


