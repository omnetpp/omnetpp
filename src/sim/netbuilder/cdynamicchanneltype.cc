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
#include "cnedloader.h"
#include "cnednetworkbuilder.h"

USING_NAMESPACE


cDynamicChannelType::cDynamicChannelType(const char *name) : cChannelType(name)
{
}

cNEDDeclaration *cDynamicChannelType::getDecl() const
{
    cNEDDeclaration *decl = cNEDLoader::instance()->getDecl(name());
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
    cNEDNetworkBuilder().addParametersTo(channel, decl);
}

