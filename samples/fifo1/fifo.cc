//-------------------------------------------------------------
// file: fifo.cc
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "omnetpp.h"
#include "fifo.h"


void AbstractFifo::activity()
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

void AbstractFifo::finish()
{
    ev << "*** Module: " << fullPath() << "***" << endl;
    ev << "Stack allocated:      " << stackSize() << " bytes";
    ev << " (includes " << ev.extraStackForEnvir() << " bytes for environment)" << endl;
    ev << "Stack actually used: " << stackUsage() << " bytes" << endl;
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
