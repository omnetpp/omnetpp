//==========================================================================
//   CDYNAMICMODULE.CC
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#include "nedelements.h"
#include "nederror.h"

#include "nedparser.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedbasicvalidator.h"
#include "nedsemanticvalidator.h"

#include "cdynamicmodule.h"
#include "cnednetworkbuilder.h"


cDynamicModuleType::cDynamicModuleType(const char *name, CompoundModuleNode *moduleNode) :
  cModuleType(name, NULL /*FIXME not good!!!*/, NULL /*FIXME not good!!!*/)
{
    modulenode = moduleNode;
}

cModule *cDynamicModuleType::createModuleObject(const char *modname, cModule *parentmod)
{
    return new cDynamicCompoundModule(modname, parentmod);
}

void cDynamicModuleType::addParametersGatesTo(cModule *mod)
{
// FIXME this should go into a cModuleInterface->addParametersGatesTo()
    // add parameters and gates to the new module, using builder
    cNEDNetworkBuilder builder;
    builder.addParametersGatesTo(mod, modulenode);
}

//FIXME this is deprecated or what....
void cDynamicModuleType::buildInside(cModule *mod)
{
    cNEDNetworkBuilder builder;
    builder.buildInside(mod, modulenode);
}

cDynamicModuleType::~cDynamicModuleType()
{
    delete modulenode;
}

cModuleInterface *cDynamicModuleType::moduleInterface()
{
    // we create no interface description for a dynamically loaded module type...
    return NULL;
}

//--------------


cDynamicCompoundModule::cDynamicCompoundModule(const char *name, cModule *parentmod) :
  cCompoundModule(name, parentmod)
{
    // FIXME TBD
}

cDynamicCompoundModule::cDynamicCompoundModule(const cDynamicCompoundModule& mod) :
  cCompoundModule(0,0)
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


