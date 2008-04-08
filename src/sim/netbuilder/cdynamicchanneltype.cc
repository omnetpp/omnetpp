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
    // do not store the pointer, because the declaration object may have been
    // thrown out of cNEDLoader to conserve memory
    cNEDDeclaration *decl = cNEDLoader::instance()->getDecl(fullName());
    ASSERT(decl->getType()==cNEDDeclaration::CHANNEL);
    return decl;
}

std::string cDynamicChannelType::info() const
{
    return getDecl()->info();
}

std::string cDynamicChannelType::detailedInfo() const
{
    return getDecl()->nedSource();
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

cProperties *cDynamicChannelType::properties() const
{
    cNEDDeclaration *decl = getDecl();
    return decl->properties();
}

cProperties *cDynamicChannelType::paramProperties(const char *paramName) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->paramProperties(paramName);
}

cProperties *cDynamicChannelType::gateProperties(const char *gateName) const
{
    throw cRuntimeError("cDynamicChannelType::gateProperties(): channels have no gates");
}

cProperties *cDynamicChannelType::submoduleProperties(const char *submoduleName, const char *submoduleType) const
{
    throw cRuntimeError("cDynamicChannelType::submoduleProperties(): channels do not contain submodules");
}

cProperties *cDynamicChannelType::connectionProperties(int connectionId, const char *channelType) const
{
    throw cRuntimeError("cDynamicChannelType::connectionProperties(): channels do not contain further connections");
}

