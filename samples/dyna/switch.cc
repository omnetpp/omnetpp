//-------------------------------------------------------------
// file: switch.cc
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "dyna.h"

class Switch : public cSimpleModule
{
    Module_Class_Members(Switch,cSimpleModule,16384)
    virtual void activity();
};

Define_Module( Switch );

void Switch::activity()
{
    double pk_delay = 1 / (double)par("pk_rate");
    for(;;)
    {
        // receive msg (implicit queueing!)
        cMessage *msg = receive();

        // processing delay
        wait( pk_delay );

        // send msg to destination
        int dest = msg->par("dest");
        ev << "Relaying msg to addr=" << dest << '\n';
        send( msg, "out", dest);
    }
}

