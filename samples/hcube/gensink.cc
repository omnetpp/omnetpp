//-------------------------------------------------------------
// File: gensink.cc
//
// Implementation of simple module types
//
// Authors: Andras Varga (TU Budapest)
//-------------------------------------------------------------

#include <stdio.h>
#include "omnetpp.h"
#include "gensink.h"

// Turn on code that prints debug messages
#define TRACE_MSG

// Module registration:
Define_Module( Sink )
Define_Module( Generator )

//
// Activities of the simple modules
//
void Generator::activity ()
{
  int num_stations = par("num_stations");
  int my_address = par("address");
  cPar& ia_time = par("ia_time"); // take by ref since it can be random

  for (int i=0;;i++)
  {
    // select destination randomly (but not the local station)
    int dest = intrand(num_stations-1);
    if (dest>=my_address) dest++;

    // create message
    char msgname[30];
    sprintf(msgname, "%d-->%d", my_address,dest);
    cMessage *msg = new cMessage(msgname);
    msg->addPar( *new cPar("src", 'L', my_address) );
    msg->addPar( *new cPar("dest", 'L', dest) );
    msg->addPar( *new cPar("hops", 'L', 0L) );
    msg->setTimestamp();

    // send out the message
#ifdef TRACE_MSG
    ev.printf("gen[%d]: Generated new msg: '%s'\n",my_address, msg->name());
#endif
    send(msg, "out");

    // wait between messages
    //
    // Note that ia_time is a reference to the module parameter "ia_time"
    // that will be evaluated here. The module parameter can also take
    // a random value (for example: truncnormal(0.5,0.1) ).
    wait( ia_time );
  }
}

int hammingDistance(unsigned long a, unsigned long b)
{
   unsigned long d = a^b;
   int k=0;
   for (; d; d=d>>1)
      if (d&1) k++;
   return k;
}

void Sink::activity()
{
  int my_address = par("address");

  cOutVector eed_vec("end-to-end delay",1);
  cOutVector hops_vec("hops",1);
  cOutVector hopratio_vec("actual/min hops ratio",1);

  for(;;)
  {
    // receive a message
    cMessage *msg = receive();

    // calculate statistics and record in output vector file
    simtime_t eed = msg->arrivalTime() - msg->timestamp();
    int acthops = msg->par("hops");
    int minhops = hammingDistance( msg->par("src"), msg->par("dest") );

    eed_vec.record( eed );
    hops_vec.record( acthops );
    hopratio_vec.record( acthops/(double)minhops );

#ifdef TRACE_MSG
    ev.printf("sink[%d]: Message received: '%s'\n", my_address, msg->name());
    ev.printf("sink[%d]:   - end-to-end delay=%lg\n", my_address, eed);
    ev.printf("sink[%d]:   - distance=%d, actual hops=%d\n", my_address, minhops, acthops);
#endif

    // message no longer needed
    delete msg;
  }
}
