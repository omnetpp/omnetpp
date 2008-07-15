//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#ifndef __ALOHA_SERVER_H_
#define __ALOHA_SERVER_H_

#include <omnetpp.h>

namespace aloha {

/**
 * Aloha server; see NED file for more info.
 */
class Server : public cSimpleModule
{
  private:
    // state variables, event pointers
    bool channelBusy;
    cMessage *endRxEvent;

    long currentCollisionNumFrames;
    simtime_t recvStartTime;

    // statistics
    long totalFrames;
    long collidedFrames;
    simtime_t totalReceiveTime;
    simtime_t totalCollisionTime;
    double currentChannelUtilization;

    cOutVector collisionMultiplicityVector;
    cOutVector collisionLengthVector;
    cOutVector channelUtilizationVector;
    cLongHistogram collisionMultiplicityHistogram;
    cDoubleHistogram collisionLengthHistogram;

  public:
    Server();
    virtual ~Server();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

}; //namespace

#endif

