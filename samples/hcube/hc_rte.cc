//-------------------------------------------------------------
// File: HcubeRte.cc
//
// Implementation of simple module types
//
// Authors: Andras Varga (TU Budapest)
//-------------------------------------------------------------

#include <stdio.h>
#include "omnetpp.h"
#include "hc_rte.h"

#define TRACE_MSG

#define SLOT_TIME   1.0
#define PROPDEL     (0.99*SLOT_TIME)

// Module registration:
Define_Module( HCRouter )

void deflectionRouting(int my_address,int dim,
                       int *rte_dest, int num_rte, int *rte_port,
                       int *usr_dest, int num_usr, int *usr_port);

//
// Activity of the simple module
//
void HCRouter::activity()
{
   int my_address = par("address");
   int dim = par("dim");
   int fromUserGateId = gate("from_gen")->id();
   long total_usr = 0, discarded_usr = 0;

   cMessage *endOfSlot = new cMessage("endOfSlot");
   for(;;)
   {
      int i;

      // buffers for transit cells (rte) and for cells from local user (usr)
      cMessage *rte_cell[32];
      int num_rte = 0;
      cMessage *usr_cell[32];
      int num_usr = 0;

      // collect cells; user cells go into separate buffer
      scheduleAt( simTime()+SLOT_TIME, endOfSlot );
      cMessage *msg;
      while ((msg=receive())!=endOfSlot)
      {
         if (msg->arrivalGateId()!=fromUserGateId)
         {
            if ((int)msg->par("dest")!=my_address)
               rte_cell[num_rte++] = msg;
            else
               send(msg,"to_sink");
         }
         else
         {
            total_usr++;
            if (num_usr<32)
               usr_cell[num_usr++] = msg;
            else
               {discarded_usr++; delete msg;}
         }
      }

      // prepare arrays used in routing
      int rte_dest[32], rte_port[32]; // destinations, output ports
      int usr_dest[32], usr_port[32];
      for (i=0;i<num_rte;i++) rte_dest[i] = rte_cell[i]->par("dest");
      for (i=0;i<num_usr;i++) usr_dest[i] = usr_cell[i]->par("dest");

      // make routing decision (functions fills rte_port[] and usr_port[])
      deflectionRouting(my_address, dim,
                        rte_dest, num_rte, rte_port,
                        usr_dest, num_usr, usr_port);

      // send out transit cells
      for (i=0; i<num_rte; i++)
      {
         rte_cell[i]->par("hops") = rte_cell[i]->par("hops") + 1;
         sendDelayed(rte_cell[i], PROPDEL, "out", rte_port[i]);
      }

      // send out user cells
      for (i=0; i<num_usr; i++)
      {
         if (usr_port[i]<0)
         {
            discarded_usr++;
            delete usr_cell[i];
         }
         else
         {
            usr_cell[i]->par("hops") = usr_cell[i]->par("hops") + 1;
            sendDelayed(usr_cell[i], PROPDEL, "out", usr_port[i]);
         }
      }
      ev.printf("rte[%d]: Discarded %ld out of %ld\n",
                my_address, discarded_usr, total_usr);
   }
}


void deflectionRouting(int my_address,int dim,
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
   for (i=0;i<num_rte;i++)
   {
      unsigned optimal = my_address ^ rte_dest[i];
      unsigned usable = optimal & ~used_ports & port_mask;

      if (usable)
      {
         // route through first usable port (with smallest index)
         int k;
         for (k=0; (usable & (1<<k))==0; k++);
         rte_port[i] = k;
         used_ports |= 1<<k;
      }
      else
      {
         // deflect through first free port (with smallest index)
         int k;
         for (k=0; used_ports & (1<<k); k++);
         rte_port[i] = k;
         used_ports |= 1<<k;
      }
   }

   // assign user cells to remaining ports
   for (i=0;i<num_usr;i++)
   {
      unsigned optimal = my_address ^ usr_dest[i];
      unsigned usable = optimal & ~used_ports & port_mask;

      if (usable)
      {
         // route through first usable port (with smallest index)
         int k;
         for (k=0; (usable & (1<<k))==0; k++);
         usr_port[i] = k;
         used_ports |= 1<<k;
      }
      else if (used_ports!=port_mask)
      {
         // deflect through first free port (with smallest index)
         int k;
         for (k=0; used_ports & (1<<k); k++);
         usr_port[i] = k;
         used_ports |= 1<<k;
      }
      else
      {
         // all ports used, cell will be discarded
         usr_port[i] = -1;
      }
   }

}
