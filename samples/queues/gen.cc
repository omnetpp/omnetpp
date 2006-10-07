//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "gen.h"


Define_Module( Generator );

void Generator::activity()
{
    int num_messages = par("num_messages");
    cPar& ia_time = par("ia_time");

    for (int i=0; i<num_messages; i++)
    {
        char msgname[32];
        sprintf( msgname, "job-%d", i);

        cMessage *msg = new cMessage( msgname );
        msg->setTimestamp();

        send( msg, "out" );

        wait( (double) ia_time );
    }
}

