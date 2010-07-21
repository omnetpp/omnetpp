//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __CHANNELCONTROLLER_H_
#define __CHANNELCONTROLLER_H_

#include <omnetpp.h>
#include "KmlHttpServer.h"
#include "KmlUtil.h"

/**
 * Interface to be implemented by mobile nodes to be able to
 * register in ChannelController.
 */
class IMobileNode
{
  public:
    virtual double getX() = 0;
    virtual double getY() = 0;
    virtual double getTxRange() = 0;
};

/**
 * This module is responsible for tracking the distance of mobile nodes
 * and visualizing the connectivity graph via KML.
 */
class ChannelController : public cSimpleModule, public IKmlFragmentProvider
{
  protected:
    static ChannelController *instance;
    std::vector<IMobileNode*> nodeList;
    double playgroundLat;
    double playgroundLon;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual std::string getKmlFragment();
    int findMobileNode(IMobileNode* p);

  public:
    ChannelController();
    virtual ~ChannelController();
    static ChannelController *getInstance();
    virtual void addMobileNode(IMobileNode* p);
    virtual void removeMobileNode(IMobileNode* p);

};

#endif
