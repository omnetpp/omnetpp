//
// fifo1.cc
//
// This file is part of an OMNeT++/OMNEST demo simulation.
//

#include "fifo1.h"


void FF1AbstractFifo::activity()
{
    msgServiced = NULL;
    endServiceMsg = new cMessage("end-service");
    queue.setName("queue");

    for(;;)
    {
        cMessage *msg = receive();
        if (msg==endServiceMsg)
        {
            endService( msgServiced );
            if (queue.empty())
            {
                msgServiced = NULL;
            }
            else
            {
                msgServiced = (cMessage *) queue.pop();
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
            queue.insert( msg );
        }
    }
}

void FF1AbstractFifo::finish()
{
    ev << "*** Module: " << fullPath() << "***" << endl;
    ev << "Stack actually used: " << stackUsage() << " bytes" << endl;
}

//------------------------------------------------

Define_Module( FF1PacketFifo );

simtime_t FF1PacketFifo::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return 1.0 / par("serviceRate").doubleValue();
}

void FF1PacketFifo::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;
    send( msg, "out" );
}


