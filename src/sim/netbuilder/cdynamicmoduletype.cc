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


cDynamicModuleType::cDynamicModuleType(const char *name) : cModuleType(name)
{
}

std::string cDynamicModuleType::info() const
{
    cNEDDeclaration *decl = cNEDLoader::instance()->lookup2(name());
    ASSERT(decl!=NULL);
    return decl->info();
}

std::string cDynamicModuleType::detailedInfo() const
{
    cNEDDeclaration *decl = cNEDLoader::instance()->lookup2(name());
    ASSERT(decl!=NULL);
    return decl->detailedInfo();
}

cNEDDeclaration *cDynamicModuleType::getDecl() const
{
    cNEDDeclaration *decl = cNEDLoader::instance()->lookup2(name());
    if (!decl)
        throw new cRuntimeError(this, "NED declaration object not found");
    //FIXME assert that it's a module decl
    return decl;
}

bool cDynamicModuleType::isNetwork() const
{
    NEDElement *tree = getDecl()->getTree();
    return tree->getTagCode()==NED_COMPOUND_MODULE && ((CompoundModuleNode *)tree)->getIsNetwork();
}

cModule *cDynamicModuleType::createModuleObject()
{
    const char *classname = getDecl()->implementationClassName();
    if (classname)
        return instantiateModuleClass(classname);
    else
        return new cCompoundModule();
}

void cDynamicModuleType::addParametersGatesTo(cModule *module)
{
    cNEDDeclaration *decl = getDecl();
    cNEDNetworkBuilder().addParameters(module, decl);
    cNEDNetworkBuilder().addGates(module, decl);
}


void cDynamicModuleType::buildInside(cModule *module)
{
    cNEDDeclaration *decl = getDecl();
    cNEDNetworkBuilder().buildInside(module, decl);
}

std::string cDynamicModuleType::declaration() const
{
    return getDecl()->declaration();
}

const char *cDynamicModuleType::extendsName() const
{
    cNEDDeclaration *decl = getDecl();
    ASSERT(decl->numExtendsNames()<=1);
    return decl->numExtendsNames()==1 ? decl->extendsName(0) : "";
}

int cDynamicModuleType::numInterfaceNames() const
{
    return getDecl()->numInterfaceNames();
}

const char *cDynamicModuleType::interfaceName(int k) const
{
    return getDecl()->interfaceName(k);
}

const char *cDynamicModuleType::implementationClassName() const
{
    return getDecl()->implementationClassName();
}

cProperties *cDynamicModuleType::properties() const
{
    return getDecl()->properties();
}

int cDynamicModuleType::numPars() const
{
    return getDecl()->numPars();
}

const char *cDynamicModuleType::parName(int k) const
{
    return getDecl()->parName(k);
}

cPar::Type cDynamicModuleType::parType(int k) const
{
    return (cPar::Type)getDecl()->paramDescription(k).value->type();
}

cProperties *cDynamicModuleType::parProperties(int k) const
{
    return getDecl()->paramDescription(k).properties;
}

int cDynamicModuleType::findPar(const char *parname) const
{
    return getDecl()->findPar(parname);
}

int cDynamicModuleType::numGates() const
{
    return getDecl()->numGates();
}

const char *cDynamicModuleType::gateName(int k) const
{
    return getDecl()->gateName(k);
}

cGate::Type cDynamicModuleType::gateType(int k) const
{
    return getDecl()->gateDescription(k).type;
}

cProperties *cDynamicModuleType::gateProperties(int k) const
{
    return getDecl()->gateDescription(k).properties;
}

int cDynamicModuleType::findGate(const char *gatename) const
{
    return getDecl()->findGate(gatename);
}

