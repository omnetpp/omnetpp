//-------------------------------------------------------------
// file: gen.cc
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "gen.h"


Define_Module( Generator )

void Generator::activity()
{
    // ia_time and msg_length are taken by reference,
    // because they can contain random values

    int num_messages = par("num_messages");
    cPar& ia_time = par("ia_time");
    cPar& msg_length = par("msg_length");

    for (int i=0; i<num_messages; i++)
    {
        char msgname[32];
        sprintf( msgname, "job-%d", i);

        ev << "Generating " << msgname << endl;
   
        cMessage *msg = new cMessage( msgname );
        msg->setLength( (long) msg_length );
        msg->setTimestamp();

        send( msg, "out" );

        wait( (double) ia_time );
    }
}

void Generator::finish()
{
    ev << "*** Module: " << fullPath() << "***" << endl;
    ev << "Stack allocated:      " << stackSize() << " bytes";
    ev << " (includes " << ev.extraStackForEnvir() << " bytes for environment)" << endl;
    ev << "Stack actually used: " << stackUsage() << " bytes" << endl;
}
