//==========================================================================
//   CDYNAMICNETWORK.CC
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

#include "cdynamicnetwork.h"
#include "cnednetworkbuilder.h"


cDynamicNetworkType::cDynamicNetworkType(const char *name, NetworkNode *networkNode) :
  cNetworkType(name)
{
    networknode = networkNode;
}

cDynamicNetworkType::~cDynamicNetworkType()
{
    delete networknode;
}


void cDynamicNetworkType::setupNetwork()
{
    cNEDNetworkBuilder builder;
    builder.setupNetwork(networknode);
}


