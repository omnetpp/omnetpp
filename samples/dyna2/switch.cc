//-------------------------------------------------------------
// file: switch.cc
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "dynapk_n.h"

class Switch : public cSimpleModule
{
    Module_Class_Members(Switch,cSimpleModule,16384)
    virtual void activity();
};

Define_Module( Switch );

void Switch::activity()
{
    double pk_delay = 1 / (double) par("pk_rate");
    for(;;)
    {
        // receive packet (implicit queueing!)
        DynaPacket *pk = (DynaPacket *) receive();

        // processing delay
        wait( pk_delay );

        // send packet to destination
        int dest = pk->getDestAddress();
        ev << "Relaying packet to addr=" << dest << '\n';
        send( pk, "out", dest);
    }
}

