//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "ChannelController.h"

Define_Module(ChannelController);

ChannelController *ChannelController::instance = NULL;

ChannelController::ChannelController()
{
    if (instance)
        error("There can be only one ChannelController instance in the network");
    instance = this;
}

ChannelController::~ChannelController()
{
    instance = NULL;
}

ChannelController *ChannelController::getInstance()
{
    if (!instance)
        throw cRuntimeError("ChannelController::getInstance(): there is no ChannelController module in the network");
    return instance;
}

int ChannelController::findMobileNode(IMobileNode* p)
{
    for (int i=0; i<(int)nodeList.size(); i++)
        if (nodeList[i] == p)
            return i;
    return -1;
}

void ChannelController::addMobileNode(IMobileNode* p)
{
    if (findMobileNode(p) == -1)
        nodeList.push_back(p);
}

void ChannelController::removeMobileNode(IMobileNode* p)
{
    int k = findMobileNode(p);
    if (k != -1)
        nodeList.erase(nodeList.begin()+k);
}

void ChannelController::initialize()
{
    playgroundLat = simulation.getSystemModule()->par("playgroundLatitude");
    playgroundLon = simulation.getSystemModule()->par("playgroundLongitude");
    KmlHttpServer::getInstance()->addKmlFragmentProvider(this);
}

void ChannelController::handleMessage(cMessage *msg)
{
    error("This module does not process messages");
}

std::string ChannelController::getKmlFragment()
{
    std::vector<KmlUtil::Pt2D> connections;

    std::string fragment;

    for (int i=0; i<(int)nodeList.size(); ++i)
    {
        for (int j=i+1; j<(int)nodeList.size(); ++j)
        {
            IMobileNode *pi = nodeList[i];
            IMobileNode *pj = nodeList[j];
            double ix = pi->getX(), iy = pi->getY(), jx = pj->getX(), jy = pj->getY();
            if (pi->getTxRange()*pi->getTxRange() > (ix-jx)*(ix-jx)+(iy-jy)*(iy-jy)) {
                double ilat = KmlUtil::y2lat(playgroundLat, iy);
                double ilon = KmlUtil::x2lon(ilat, playgroundLon, ix);
                connections.push_back(KmlUtil::Pt2D(ilon, ilat));

                double jlat = KmlUtil::y2lat(playgroundLat, jy);
                double jlon = KmlUtil::x2lon(jlat, playgroundLon, jx);
                connections.push_back(KmlUtil::Pt2D(jlon, jlat));
            }
        }
    }
    fragment += KmlUtil::lines("connectivity_1", connections, "connectivity graph", NULL, "60FFFFFF");

    return fragment;
}


