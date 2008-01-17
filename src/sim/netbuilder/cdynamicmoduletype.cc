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
    return getDecl()->detailedInfo();
}

cNEDDeclaration *cDynamicModuleType::getDecl() const
{
    cNEDDeclaration *decl = cNEDLoader::instance()->getDecl(name());
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


