//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef ROUTING_APP_H
#define ROUTING_APP_H

#include <vector>
#include <omnetpp.h>
#include "Packet_m.h"

using namespace omnetpp;

/**
 * Generates traffic for the network.
 */
class App2 : public cSimpleModule
{
  private:
    // configuration
    int myAddress;
    std::vector<int> destAddresses;
    std::vector<double> sendIATimes;
    int nextIaTimeIndex = 0;
    cPar *packetLengthBytes;

    // state
    cMessage *generatePacketTimer = nullptr;
    long pkCounter;

    // signals
    simsignal_t endToEndDelaySignal;
    simsignal_t hopCountSignal;
    simsignal_t sourceAddressSignal;

  public:
    virtual ~App2();
    virtual bool isRunning();
    virtual void start();
    virtual void stop();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void generatePacket();
    virtual void processReceivedPacket(Packet *pk);
    virtual void shout(const char *s);
    virtual double getIaTime();
};

#endif

