#include <stdio.h>
#include "omnetpp.h"

class smSender : public cSimpleModule
{
     Module_Class_Members(smSender,cSimpleModule,8192)
     virtual void activity();
};

Define_Module( smSender )

void smSender::activity()
{
        int i;

        ev << "Sender module started.\n";
        for (i = 0; i < (int)par("no_msg"); i++)
        {
            syncpoint( simTime()+2.0, "outgate" );

            wait( 2.0 );

            ev << "Sending msg #" << i << ".\n";
            cMessage *msg = new cMessage("test-msg");
            msg->addPar("serial") = (long)i;
            send (msg, "outgate");
        }
        ev << "Sender done!\n";
}

//----------------------------------------------------

class smReceiver : public cSimpleModule
{
     Module_Class_Members(smReceiver,cSimpleModule,8192)
     virtual void activity();
};

Define_Module( smReceiver )

void smReceiver::activity()
{
        ev << "Receiver started.\n";
        double timeout=5.0;
        for (;;)
        {
            cMessage *msg = receive( timeout );
            if (!msg) break;
            ev << "cMessage received, name: " << msg->name() << "\n";
            ev << "  parameter 'serial'=" << (long)msg->par("serial") << "\n";
            delete msg;
        }
        ev << "Receiver: Timeout expired, stopping simulation.\n";
        endSimulation();
}

