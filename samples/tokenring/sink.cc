//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Authors: Gabor Lencse, Andras Varga (TU Budapest)
// Based on the code by:
//          Maurits Andre, George van Montfort,
//          Gerard van de Weerd (TU Delft)
//

#include <omnetpp.h>
#include "token_m.h"


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

    bool debug;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module( Sink );


void Sink::initialize()
{
    endToEndDelay.setName("End-to-End Delay");

    endToEndDelayKS.setName("End-to-End Delay histogram (K-split)");
    endToEndDelayKS.setRangeAutoUpper(0.0, 100, 2.0);
    endToEndDelayPS.setName("End-to-End Delay histogram (P2)");

    debug=true;
    WATCH(debug);
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t eed = simTime() - msg->creationTime();
    if (debug)
    {
        ev << "Received app. data: \"" << msg->name() << "\", "
              "length=" << msg->length()/8 << "bytes, " <<
              "end-to-end delay=" << simtimeToStr(eed) << endl;
    }

    // record statistics to output vector file and histograms
    endToEndDelay.record(eed);
    endToEndDelayKS.collect(eed);
    endToEndDelayPS.collect(eed);

    // message no longer needed
    delete msg;
}

void Sink::finish()
{
    endToEndDelayKS.recordScalar();
}

