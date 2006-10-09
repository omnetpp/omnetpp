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
#include "cnedresourcecache.h"
#include "cnednetworkbuilder.h"


cDynamicChannelType::cDynamicChannelType(const char *name) : cChannelType(name)
{
}

cChannel *cDynamicChannelType::createChannelObject()
{
    cNEDDeclaration *decl = cNEDResourceCache::instance()->lookup2(name());
    ASSERT(decl!=NULL);
//FIXME assert that it's a channel
    const char *classname = decl->implementationClassName();
    ASSERT(classname!=NULL);
    return instantiateChannelClass(classname);
}

void cDynamicChannelType::addParametersTo(cChannel *channel)
{
    cNEDDeclaration *decl = cNEDResourceCache::instance()->lookup2(name());
    ASSERT(decl!=NULL);
    cNEDNetworkBuilder().addParameters(channel, decl);
}



