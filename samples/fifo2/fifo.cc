//-------------------------------------------------------------
// file: fifo.cc
//        (part of Fifo2 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "omnetpp.h"
#include "fifo.h"


void FF2AbstractFifo::initialize()
{
    msgServiced = NULL;
    endServiceMsg = new cMessage("end-service");
    queue.setName("queue");
}

void FF2AbstractFifo::handleMessage(cMessage *msg)
{
    if (msg==endServiceMsg)
    {
        endService( msgServiced );
        if (queue.empty())
        {
            msgServiced = NULL;
        }
        else
        {
            msgServiced = (cMessage *) queue.getTail();
            simtime_t serviceTime = startService( msgServiced );
            scheduleAt( simTime()+serviceTime, endServiceMsg );
        }
    }
    else if (!msgServiced)
    {
        arrival( msg );
        msgServiced = msg;
        simtime_t serviceTime = startService( msgServiced );
        scheduleAt( simTime()+serviceTime, endServiceMsg );

    }
    else
    {
        arrival( msg );
        queue.insertHead( msg );
    }
}

//------------------------------------------------

Define_Module( ACPFifo )

simtime_t ACPFifo::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return par("service_time");
}

void ACPFifo::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;
    send( msg, "out" );
}

//------------------------------------------------

Define_Module( ACBFifo )

simtime_t ACBFifo::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return msg->length() / par("bits_per_sec");
}

void ACBFifo::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;
    send( msg, "out" );
}
