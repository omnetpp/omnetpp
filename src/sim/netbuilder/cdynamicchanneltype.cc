//==========================================================================
//   CDYNAMICCHANNELTYPE.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

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
    cNEDDeclaration *decl = cNEDLoader::getInstance()->getDecl(getFullName());
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

cProperties *cDynamicChannelType::getProperties() const
{
    cNEDDeclaration *decl = getDecl();
    return decl->getProperties();
}

cProperties *cDynamicChannelType::getParamProperties(const char *paramName) const
{
    cNEDDeclaration *decl = getDecl();
    return decl->getParamProperties(paramName);
}

cProperties *cDynamicChannelType::getGateProperties(const char *gateName) const
{
    throw cRuntimeError("cDynamicChannelType::getGateProperties(): channels have no gates");
}

cProperties *cDynamicChannelType::getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const
{
    throw cRuntimeError("cDynamicChannelType::getSubmoduleProperties(): channels do not contain submodules");
}

cProperties *cDynamicChannelType::getConnectionProperties(int connectionId, const char *channelType) const
{
    throw cRuntimeError("cDynamicChannelType::getConnectionProperties(): channels do not contain further connections");
}

