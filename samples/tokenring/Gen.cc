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

#define STACKSIZE 16384


/**
 * Generates traffic; see NED file for more info.
 */
class Generator : public cSimpleModule
{
  public:
    Generator() : cSimpleModule(STACKSIZE) {}
    virtual void activity();
};

Define_Module(Generator);


void Generator::activity()
{
    int numMessages = par("numMessages");
    int numStations = par("numStations");
    cPar& interArrivalTime = par("interArrivalTime"); // take by ref since it can be random
    cPar& messageLength = par("messageLength"); // take by ref since it can be random
    int myAddress = par("address");

    for (int i=0; i<numMessages; i++)
    {
        // select length of data (bytes)
        int length = (int)messageLength;

        // select a destination randomly (but not the local station)
        int dest = intrand(numStations-1);
        if (dest>=myAddress) dest++;

        // create message
        char msgname[30];
        sprintf(msgname, "app%d-data%d", myAddress, i);
        TRApplicationData *msg = new TRApplicationData(msgname);
        msg->setDestination(dest);
        msg->setByteLength(length);
        msg->setData("here's some application data...");

        // send message on gate "out", which is connected to the Token Ring MAC
        EV << "Generated application data to send: \"" << msgname << "\", "
              "length=" << length << " bytes, dest=" << dest << endl;
        send(msg, "out");

        // wait between messages. Note that interArrivalTime is a reference to the module
        // parameter "interArrivalTime" which will be evaluated here. The module parameter
        // can be set to a random variate (for example: truncnormal(0.5,0.1)),
        // and then we'll get random delay here.
        wait( interArrivalTime.doubleValue() );
    }
}

