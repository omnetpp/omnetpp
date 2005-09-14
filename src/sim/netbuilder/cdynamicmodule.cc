//==========================================================================
//   CDYNAMICMODULE.CC
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

#include "cdynamicmodule.h"
#include "cnednetworkbuilder.h"


cDynamicModuleType::cDynamicModuleType(const char *name, CompoundModuleNode *moduleNode) :
  cModuleType(name, name, NULL)
{
    modulenode = moduleNode;
}

cModule *cDynamicModuleType::createModuleObject()
{
    return new cDynamicCompoundModule();
}

void cDynamicModuleType::buildInside(cModule *mod)
{
    cNEDNetworkBuilder builder;
    builder.buildInside(mod, modulenode);
}

cDynamicModuleType::~cDynamicModuleType()
{
    delete modulenode;
}

//--------------


cDynamicCompoundModule::cDynamicCompoundModule() 
{
}

cDynamicCompoundModule::cDynamicCompoundModule(const cDynamicCompoundModule& mod)
{
    operator=(mod);
}

const char *cDynamicCompoundModule::className() const
{
    cModuleType *modtype = moduleType();
    return modtype ? modtype->name() : cCompoundModule::className(); //"type n/a yet";
}

cDynamicCompoundModule& cDynamicCompoundModule::operator=(const cDynamicCompoundModule& mod)
{
    if (this==&mod)
        return *this;

    operator=(mod);
    return *this;
}


void cDynamicCompoundModule::doBuildInside()
{
    // ask module type to build our internal structure
    moduleType()->buildInside(this);
}


