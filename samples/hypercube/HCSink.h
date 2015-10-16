//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __HCSINK_H_
#define __HCSINK_H_

#include <omnetpp.h>

using namespace omnetpp;

/**
 * Packet sink; see NED file for more info.
 */
class HCSink : public cSimpleModule
{
  private:
    simsignal_t endToEndDelaySignal;
    simsignal_t hopCountSignal;
    simsignal_t hopRatioSignal;

  public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif

