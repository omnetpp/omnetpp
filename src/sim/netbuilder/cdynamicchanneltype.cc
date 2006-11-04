//==========================================================================
//   CDYNAMICCHANNELTYPE.CC
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

#include "cdynamicchanneltype.h"
#include "cneddeclaration.h"
#include "cnedloader.h"
#include "cnednetworkbuilder.h"


cDynamicChannelType::cDynamicChannelType(const char *name) : cChannelType(name)
{
}

cNEDDeclaration *cDynamicChannelType::getDecl() const
{
    cNEDDeclaration *decl = cNEDLoader::instance()->lookup2(name());
    if (!decl)
        throw new cRuntimeError(this, "NED declaration object not found");
    //FIXME assert that it's a channel decl
    return decl;
}

std::string cDynamicChannelType::info() const
{
    return getDecl()->info();
}

std::string cDynamicChannelType::detailedInfo() const
{
    return getDecl()->detailedInfo();
}

cChannel *cDynamicChannelType::createChannelObject()
{
    const char *classname = getDecl()->implementationClassName();
    return instantiateChannelClass(classname);
}

void cDynamicChannelType::addParametersTo(cChannel *channel)
{
    cNEDDeclaration *decl = getDecl();
    cNEDNetworkBuilder().addParameters(channel, decl);
}

std::string cDynamicChannelType::declaration() const
{
    return getDecl()->declaration();
}

const char *cDynamicChannelType::extendsName() const
{
    cNEDDeclaration *decl = getDecl();
    ASSERT(decl->numExtendsNames()<=1);
    return decl->numExtendsNames()==1 ? decl->extendsName(0) : "";
}

int cDynamicChannelType::numInterfaceNames() const
{
    return getDecl()->numInterfaceNames();
}

const char *cDynamicChannelType::interfaceName(int k) const
{
    return getDecl()->interfaceName(k);
}

const char *cDynamicChannelType::implementationClassName() const
{
    return getDecl()->implementationClassName();
}

cProperties *cDynamicChannelType::properties() const
{
    return getDecl()->properties();
}

int cDynamicChannelType::numPars() const
{
    return getDecl()->numPars();
}

const char *cDynamicChannelType::parName(int k) const
{
    return getDecl()->parName(k);
}

cPar::Type cDynamicChannelType::parType(int k) const
{
    return (cPar::Type)getDecl()->paramDescription(k).value->type();
}

cProperties *cDynamicChannelType::parProperties(int k) const
{
    return getDecl()->paramDescription(k).properties;
}

int cDynamicChannelType::findPar(const char *parname) const
{
    return getDecl()->findPar(parname);
}

