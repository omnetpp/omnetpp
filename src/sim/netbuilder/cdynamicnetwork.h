//==========================================================================
//   CDYNAMICNETWORK.H -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDYNAMICNETWORK_H
#define __CDYNAMICNETWORK_H

#include "cmodule.h"
#include "ctypes.h"

#include "cnednetworkbuilder.h"


/**
 * Network registration object, to be used with dynamic NED loading.
 * See cDynamicModuleType for more information.
 */
class cDynamicNetworkType : public cNetworkType
{
  protected:
    NetworkNode *networknode;  // contains NEDElement tree

  public:
    cDynamicNetworkType(const char *name, NetworkNode *networkNode);
    virtual ~cDynamicNetworkType();

    /**
     * Network setup function that builds upon cNEDNetworkBuilder.
     */
    virtual void setupNetwork();
};

#if 0
/**
 * Channel registration object, to be used with dynamic NED loading.
 * See cDynamicModuleType for more information.
 */
class cDynamicChannelType : public cChannelType
{
  protected:
    ChannelNode *channelnode;  // contains NEDElement tree

  public:
    cDynamicChannelType(const char *name, ChannelNode *channelNode);
    virtual cDynamicChannelType();

    /**
     * Factory method that creates a channel instance.
     */
    virtual cChannel *create();
};
#endif

#endif


