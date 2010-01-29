//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#ifndef __HCSINK_H_
#define __HCSINK_H_

#include <omnetpp.h>

#define STACKSIZE 16384

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
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif

