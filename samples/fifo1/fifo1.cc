//-------------------------------------------------------------
// file: fifo1.cc
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "fifo1.h"


void FF1AbstractFifo::activity()
{
    msgServiced = NULL;
    endServiceMsg = new cMessage("end-service");

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
}

void FF1AbstractFifo::finish()
{
    ev << "*** Module: " << fullPath() << "***" << endl;
    ev << "Stack allocated:      " << stackSize() << " bytes";
    ev << " (includes " << ev.extraStackForEnvir() << " bytes for environment)" << endl;
    ev << "Stack actually used: " << stackUsage() << " bytes" << endl;
}

//------------------------------------------------

Define_Module( FF1PacketFifo );

simtime_t FF1PacketFifo::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return par("service_time");
}

void FF1PacketFifo::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;
    send( msg, "out" );
}

//------------------------------------------------

Define_Module( FF1BitFifo );

simtime_t FF1BitFifo::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return msg->length() / (double)par("bits_per_sec");
}

void FF1BitFifo::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;
    send( msg, "out" );
}

