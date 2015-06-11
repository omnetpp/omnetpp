//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "HCRouter.h"
#include "HCPacket_m.h"

Define_Module(HCRouter);

#define SLOT_TIME    1.0
#define PROPDEL      (0.99*SLOT_TIME)

void deflectionRouting(int my_address, int dim,
        int *rte_dest, int num_rte, int *rte_port,
        int *usr_dest, int num_usr, int *usr_port)
{
    // This function implements a simple, fast, but very suboptimal, unfair,
    // unbalanced etc. deflection scheme.
    int i;

    unsigned port_mask = (1<<dim)-1;

    // used_ports: bits set to 1 will denote ports already assigned
    unsigned used_ports = 0;

    // assign transit cells to ports
    for (i = 0; i < num_rte; i++) {
        unsigned optimal = my_address ^ rte_dest[i];
        unsigned usable = optimal & ~used_ports & port_mask;

        if (usable) {
            // route through first usable port (with smallest index)
            int k;
            for (k = 0; (usable & (1<<k)) == 0; k++)
                ;
            rte_port[i] = k;
            used_ports |= 1<<k;
        }
        else {
            // deflect through first free port (with smallest index)
            int k;
            for (k = 0; used_ports &(1<<k); k++)
                ;
            rte_port[i] = k;
            used_ports |= 1<<k;
        }
    }

    // assign user cells to remaining ports
    for (i = 0; i < num_usr; i++) {
        unsigned optimal = my_address ^ usr_dest[i];
        unsigned usable = optimal & ~used_ports & port_mask;

        if (usable) {
            // route through first usable port (with smallest index)
            int k;
            for (k = 0; (usable & (1<<k)) == 0; k++)
                ;
            usr_port[i] = k;
            used_ports |= 1<<k;
        }
        else if (used_ports != port_mask) {
            // deflect through first free port (with smallest index)
            int k;
            for (k = 0; used_ports &(1<<k); k++)
                ;
            usr_port[i] = k;
            used_ports |= 1<<k;
        }
        else {
            // all ports used, cell will be discarded
            usr_port[i] = -1;
        }
    }
}

void HCRouter::activity()
{
    int my_address = par("address");
    int dim = par("dim");
    int fromUserGateId = gate("fromGen")->getId();
    long total_usr = 0, discarded_usr = 0;

    cMessage *endOfSlot = new cMessage("endOfSlot");
    for ( ; ; ) {
        int i;

        // buffers for transit cells (rte) and for cells from local user (usr)
        HCPacket *rte_cell[32];
        int num_rte = 0;
        HCPacket *usr_cell[32];
        int num_usr = 0;

        // collect cells; user cells go into separate buffer
        scheduleAt(simTime()+SLOT_TIME, endOfSlot);
        cMessage *msg;
        while ((msg = receive()) != endOfSlot) {
            HCPacket *pkt = check_and_cast<HCPacket *>(msg);
            if (pkt->getArrivalGateId() != fromUserGateId) {
                if (pkt->getDestAddress() != my_address)
                    rte_cell[num_rte++] = pkt;
                else
                    send(pkt, "toSink");
            }
            else {
                total_usr++;
                if (num_usr < 32)
                    usr_cell[num_usr++] = pkt;
                else {
                    discarded_usr++;
                    delete pkt;
                }
            }
        }

        // prepare arrays used in routing
        int rte_dest[32], rte_port[32];  // destinations, output ports
        int usr_dest[32], usr_port[32];
        for (i = 0; i < num_rte; i++)
            rte_dest[i] = rte_cell[i]->getDestAddress();
        for (i = 0; i < num_usr; i++)
            usr_dest[i] = usr_cell[i]->getDestAddress();

        // make routing decision (function fills rte_port[] and usr_port[])
        deflectionRouting(my_address, dim,
                rte_dest, num_rte, rte_port,
                usr_dest, num_usr, usr_port);

        // send out transit cells
        for (i = 0; i < num_rte; i++) {
            rte_cell[i]->setHops(rte_cell[i]->getHops()+1);
            sendDelayed(rte_cell[i], PROPDEL, "out", rte_port[i]);
        }

        // send out user cells
        for (i = 0; i < num_usr; i++) {
            if (usr_port[i] < 0) {
                discarded_usr++;
                delete usr_cell[i];
            }
            else {
                usr_cell[i]->setHops(usr_cell[i]->getHops()+1);
                sendDelayed(usr_cell[i], PROPDEL, "out", usr_port[i]);
            }
        }
        EV << "rte[" << my_address << "]: Discarded " << discarded_usr << " out of " << total_usr << "\n";
    }
}

