//-------------------------------------------------------------
// file: switch.cc
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "dynapacket_m.h"

class Switch : public cSimpleModule
{
    Module_Class_Members(Switch,cSimpleModule,16384)
    virtual void activity();
};

Define_Module( Switch );

void Switch::activity()
{
    double pk_delay = 1 / (double)par("pk_rate");
    int queue_max_len = (int) par("queue_max_len");
    cQueue queue("queue");
    for(;;)
    {
        // receive msg
        cMessage *msg;
        if (!queue.empty())
            msg = (cMessage *) queue.pop();
        else
            msg = receive();

        // model processing delay; packets that arrive meanwhile are queued
        waitAndEnqueue(pk_delay, &queue);

        // send msg to destination
        DynaPacket *pk = check_and_cast<DynaPacket *>(msg);
        int dest = pk->getDestAddress();
        ev << "Relaying msg to addr=" << dest << '\n';
        send( msg, "out", dest);

        // model finite queue size
        while (queue.length() > queue_max_len)
        {
            ev << "Buffer overflow, discarding " << queue.tail()->name() << endl;
            delete queue.pop();
        }
    }
}


