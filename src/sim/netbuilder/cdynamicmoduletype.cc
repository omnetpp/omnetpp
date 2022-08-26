//==========================================================================
//   CDYNAMICMODULETYPE.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cstdio>
#include <ctime>
#include <iostream>

#include "cdynamicmoduletype.h"
#include "cneddeclaration.h"
#include "cnedloader.h"
#include "cnednetworkbuilder.h"

namespace omnetpp {

cDynamicModuleType::cDynamicModuleType(const char *name) : cModuleType(name)
{
}

cNedDeclaration *cDynamicModuleType::getDecl() const
{
    // do not store the pointer, because the declaration object may have been
    // thrown out of cNedLoader to conserve memory
    cNedDeclaration *decl = cNedLoader::getInstance()->getDecl(getFullName());
    ASSERT(decl->getType() == cNedDeclaration::SIMPLE_MODULE || decl->getType() == cNedDeclaration::COMPOUND_MODULE);
    return decl;
}

std::string cDynamicModuleType::str() const
{
    return getDecl()->str();
}

std::string cDynamicModuleType::getNedSource() const
{
    return getDecl()->getNedSource();
}

bool cDynamicModuleType::isNetwork() const
{
    return getDecl()->isNetwork();
}

bool cDynamicModuleType::isSimple() const
{
    return getDecl()->getType() == cNedDeclaration::SIMPLE_MODULE;
}

cModule *cDynamicModuleType::createModuleObject()
{
    const char *classname = getDecl()->getImplementationClassName();
    ASSERT(classname != nullptr);
    return instantiateModuleClass(classname);
}

void cDynamicModuleType::addParametersAndGatesTo(cModule *module)
{
    cNedDeclaration *decl = getDecl();
    cNedNetworkBuilder().addParametersAndGatesTo(module, decl);
}

void cDynamicModuleType::applyPatternAssignments(cComponent *component)
{
    cNedNetworkBuilder().assignParametersFromPatterns(component);
}

void cDynamicModuleType::setupGateVectors(cModule *module)
{
    cNedDeclaration *decl = getDecl();
    cNedNetworkBuilder().setupGateVectors(module, decl);
}

void cDynamicModuleType::buildInside(cModule *module)
{
    cNedDeclaration *decl = getDecl();
    cNedNetworkBuilder().buildInside(module, decl);
}

cProperties *cDynamicModuleType::getProperties() const
{
    cNedDeclaration *decl = getDecl();
    return decl->getProperties();
}

cProperties *cDynamicModuleType::getParamProperties(const char *paramName) const
{
    cNedDeclaration *decl = getDecl();
    return decl->getParamProperties(paramName);
}

cProperties *cDynamicModuleType::getGateProperties(const char *gateName) const
{
    cNedDeclaration *decl = getDecl();
    return decl->getGateProperties(gateName);
}

cProperties *cDynamicModuleType::getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const
{
    cNedDeclaration *decl = getDecl();
    return decl->getSubmoduleProperties(submoduleName, submoduleType);
}

cProperties *cDynamicModuleType::getConnectionProperties(int connectionId, const char *channelType) const
{
    cNedDeclaration *decl = getDecl();
    return decl->getConnectionProperties(connectionId, channelType);
}

std::string cDynamicModuleType::getPackageProperty(const char *name) const
{
    cNedDeclaration *decl = getDecl();
    return decl->getPackageProperty(name);
}

const char *cDynamicModuleType::getImplementationClassName() const
{
    cNedDeclaration *decl = getDecl();
    return decl->getImplementationClassName();
}

std::string cDynamicModuleType::getCxxNamespaceForType(const char *type) const
{
    cNedDeclaration *decl = cNedLoader::getInstance()->getDecl(type);
    return decl->getPackageProperty("namespace");
}

std::string cDynamicModuleType::getCxxNamespace() const
{
    cNedDeclaration *decl = getDecl();
    return decl->getCxxNamespace();
}

const char *cDynamicModuleType::getSourceFileName() const
{
    cNedDeclaration *decl = getDecl();
    return decl->getSourceFileName();
}

bool cDynamicModuleType::isInnerType() const
{
    cNedDeclaration *decl = getDecl();
    return decl->isInnerType();
}

void cDynamicModuleType::clearSharedParImpls()
{
    cModuleType::clearSharedParImpls();
    getDecl()->clearSharedParImpls();
}

}  // namespace omnetpp

