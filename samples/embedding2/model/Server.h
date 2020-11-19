//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __ALOHA_SERVER_H_
#define __ALOHA_SERVER_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace aloha {

/**
 * Aloha server; see NED file for more info.
 */
class Server : public cSimpleModule
{
  private:
    // state variables, event pointers
    bool channelBusy;
    cMessage *endRxEvent = nullptr;

    intval_t currentCollisionNumFrames;
    simtime_t recvStartTime;

    // statistics
    intval_t totalFrames;
    intval_t collidedFrames;
    simtime_t totalReceiveTime;
    simtime_t totalCollisionTime;
    double currentChannelUtilization;

    cOutVector collisionMultiplicityVector;
    cOutVector collisionLengthVector;
    cOutVector channelUtilizationVector;
    cHistogram collisionMultiplicityHistogram;
    cHistogram collisionLengthHistogram;

  public:
    virtual ~Server();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

}; //namespace

#endif

