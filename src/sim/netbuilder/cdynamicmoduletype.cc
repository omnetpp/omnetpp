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
#include "cnedresourcecache.h"
#include "cnednetworkbuilder.h"


cDynamicModuleType::cDynamicModuleType(const char *name) : cModuleType(name)
{
}

cModule *cDynamicModuleType::createModuleObject()
{
    cNEDDeclaration *decl = cNEDResourceCache::instance()->lookup2(name());
    ASSERT(decl!=NULL);
//FIXME assert that it's a module decl
    const char *classname = decl->implementationClassName();
    if (classname)
        return instantiateModuleClass(classname);
    else
        return new cCompoundModule();
}

void cDynamicModuleType::addParametersGatesTo(cModule *module)
{
    cNEDDeclaration *decl = cNEDResourceCache::instance()->lookup2(name());
    ASSERT(decl!=NULL);
    cNEDNetworkBuilder().addParameters(module, decl);
    cNEDNetworkBuilder().addGates(module, decl);
}


void cDynamicModuleType::buildInside(cModule *module)
{
    cNEDDeclaration *decl = cNEDResourceCache::instance()->lookup2(name());
    ASSERT(decl!=NULL);
    cNEDNetworkBuilder().buildInside(module, decl);
}


