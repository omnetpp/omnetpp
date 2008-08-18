//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Authors: Gabor Lencse, Andras Varga
// Based on the code by:
//          Maurits Andre, George van Montfort,
//          Gerard van de Weerd (TU Delft)
//

#include <omnetpp.h>
#include "TokenRing_m.h"


/**
 * Consumes packets; see NED file for more info.
 */
class Sink : public cSimpleModule
{
  private:
    // output vector to record statistics
    cOutVector endToEndDelay;

    // histograms
    cKSplit endToEndDelayKS;
    cPSquare endToEndDelayPS;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module(Sink);


void Sink::initialize()
{
    endToEndDelay.setName("End-to-End Delay");

    endToEndDelayKS.setName("End-to-End Delay histogram (K-split)");
    endToEndDelayKS.setRangeAutoUpper(0.0, 100, 2.0);
    endToEndDelayPS.setName("End-to-End Delay histogram (P2)");
}

void Sink::handleMessage(cMessage *msg)
{
    cPacket *pkt = check_and_cast<cPacket *>(msg);
    simtime_t eed = simTime() - pkt->getCreationTime();
    EV << "Received app. data: \"" << pkt->getName() << "\", "
          "length=" << pkt->getByteLength() << "bytes, " <<
          "end-to-end delay=" << eed << endl;

    // record statistics to output vector file and histograms
    endToEndDelay.record(eed);
    endToEndDelayKS.collect(eed);
    endToEndDelayPS.collect(eed);

    // packet no longer needed
    delete pkt;
}

void Sink::finish()
{
    endToEndDelayKS.record();
}

