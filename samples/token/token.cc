//-------------------------------------------------------------
// File: token.cc
//
// Implementation of simple module types
//
// Authors: Gabor Lencse, Andras Varga (TU Budapest)
// Based on the code by:
//          Maurits Andr‚, George van Montfort,
//          Gerard van de Weerd (TU Delft)
//-------------------------------------------------------------

#include <stdio.h>
#include "omnetpp.h"
#include "token.h"

// Turn on code that prints debug messages
#define TRACE_MSG

// Message kind values
#define DATA_FRAME   0
#define TOKEN        1

// Module registration:
Define_Module( Sink )
Define_Module( Generator )
Define_Module( TokenRingMAC )


//
// Activities of the simple modules
//
void Generator::activity ()
{
  char msgname[30];

  int num_messages = par("num_messages");
  int num_stations = par("num_stations");
  cPar& ia_time = par("ia_time"); // take by ref since it can be random

  int my_address = parentModule()->index();

  for (int i = 0; i < num_messages; i++)
  {
    // select destination randomly (but not the local station)
    int dest = intrand(num_stations-1);
    if (dest>=my_address) dest++;

    // select packet length
    //
    // Fields of a Token Ring data frame are (field lengths given in octets):
    //   SD(1), AC(1), FC(1), DEST(6), SOURCE(6), DATA(n), CS(4), ED(1), FS(1)
    // Length of DATA field (n) is bounded only by the Token Holding Time.
    // Typical THT value is 10ms, thus
    //       length [bits] <= THT*DATARATE = 10ms*4Mbit/s = 5000 bytes
    long length = (long) intuniform(21, 5000);

    // create message
    sprintf(msgname, "%d-->%d", my_address,dest);
    cMessage *msg = new cMessage(msgname, DATA_FRAME);
    msg->addPar("dest") = (long) dest;
    msg->addPar("source") = (long) my_address;
    msg->setLength(8*length); // the length is measured in bits

    // pass down the message for Token Ring MAC
#ifdef TRACE_MSG
    ev.printf("gen[%d]: Generated new msg: ",my_address);
    ev.printf("         Name: %s\n",msg->name());
#endif
    send(msg, "out");

    // wait between messages
    //
    // Note that ia_time is a reference to the module parameter "ia_time"
    // that will be evaluated here. The module parameter can also be a random
    // value (for example: truncnormal(0.5,0.1) ).
    wait( ia_time );
  }
}

void TokenRingMAC::activity()
{
  cQueue send_queue("send-queue");
  int my_address = parentModule()->index();
  cOutVector sendqueue_len("sendqueue-length",1);

  long datarate = par("data_rate");   // 4 or 16 Mbit/s
  double THT = par("THT");            // Token Holding Time: typically 10ms

  // station 0 issues the token
  if (my_address == 0)
  {
    cMessage *token = new cMessage("token", TOKEN);
    token->setLength(8*3);   //IEEE 802.5: token length is 3 bytes
    send( token, "phy_out");
  }

  // enter token ring protocol
  for(;;)
  {
    cMessage *msg = receive();

    // new message from the generator
    if ( msg->arrivedOn("from_gen") ) // DATA_FRAME
    {
#ifdef TRACE_MSG
      ev.printf("MAC[%d]: New msg from higher layer: ",my_address);
      ev.printf("         Name: %s\n",msg->name());
      ev.printf("MAC[%d]:   - adding to send-queue\n",my_address);
#endif

      // insert message into send buffer
      send_queue.insertHead( msg );
      sendqueue_len.record( send_queue.length() );
    }

    // token arrived from network
    else if ( msg->arrivedOn("phy_in") && msg->kind()==TOKEN )
    {
#ifdef TRACE_MSG
      ev.printf("MAC[%d]: Token arrived: ",my_address);
      ev.printf("         Name: %s\n",msg->name());
#endif

      // if we have messages to send, send them now, within the THT
      //
      // We'll use wait() for modelling the transmission time. This means
      // that we do not process messages that arrive in the meanwhile;
      // they accumulate in the putaside-queue and will be retrieved
      // by subsequent receive() calls.
      //
      // The exactness of the simulation is not compromised this way,
      // because these messages can be of two kind:
      //  1. new messages from the generator. They will be sent with the
      //     next token.
      //  2. our frames that travelled around the ring and arrived back to us.
      //     We'll have to strip them out from the ring anyway so they
      //     can wait a little.
      simtime_t tht_expires = simTime()+THT;
      while (!send_queue.empty())
      {
        // peek at next message and see if it is within the THT
        cMessage *m = (cMessage *) send_queue.peekTail();
        simtime_t transmission_time = m->length()/(double)datarate;
        if (simTime()+transmission_time > tht_expires)
          break; // no time left for this packet

        // remove message from the send buffer
        send_queue.getTail();
        sendqueue_len.record( send_queue.length() );

        // add delivery time and send it
        m->addPar("sendtime") = (double) simTime();
#ifdef TRACE_MSG
        ev.printf("MAC[%d]: Sending frame from send-queue: ",my_address);
        ev.printf("         Name: %s\n", m->name());
        ev.printf("MAC[%d]:  - transmission time: %s. (During transm, messages\n",
                  my_address, simtimeToStr(transmission_time));
        ev.printf("MAC[%d]:    from higher layer are temporarily stored in the putaside-queue)\n",
                  my_address);
#endif
        send( m, "phy_out");
        wait( transmission_time );
      }

      // release token
#ifdef TRACE_MSG
      ev.printf("MAC[%d]: Releasing token.\n",my_address);
#endif
      send( msg, "phy_out");
    }

    // frame arrived from network
    else if ( msg->arrivedOn("phy_in") && msg->kind() == DATA_FRAME )
    {
#ifdef TRACE_MSG
      ev.printf("MAC[%d]: Frame from network: ",my_address);
      ev.printf("         Name: %s\n",msg->name());
#endif

      // extract source and destination
      int dest = (long) msg->par ("dest");
      int source = (long) msg->par ("source");

      // is this station the addressee?
      if (dest == my_address)
      {
        // The arrival of a message in the model represents the arrival
        // of the first bit of the packet. It has to be completely received
        // before we can pass it up to the higher layer. This delay is
        // implemented here with a delayed send.
        //
        // create a copy and send it up
        cMessage *m = (cMessage *) msg->dup();
#ifdef TRACE_MSG
        ev.printf("MAC[%d]:  - we are the destination, pass up a copy to higher layer.\n",my_address);
#endif
        sendDelayed( m,m->length()/(double)datarate,"to_sink");
      }

      // In the Token Ring protocol, a frame is stripped out from the ring
      // by the source of the frame when the frame arrives back to its
      // originating station. The addresse just repeats the frame, like
      // any other station in the ring.
      if (source == my_address)
      {
#ifdef TRACE_MSG
        ev.printf("MAC[%d]:  - arrived back to sender, strip it out of the ring.\n",my_address);
#endif
        delete msg;
      }
      else
      {
#ifdef TRACE_MSG
        ev.printf("MAC[%d]:  - forward it to next station.\n",my_address);
#endif
        send( msg, "phy_out");
      }
    }
  }
}

void Sink::activity()
{
  int my_address = parentModule()->index();

  cOutVector queuing_time("queuing-time",1);
  cOutVector transm_time("transmission-time",1);

  for(;;)
  {
    // receive a message
    cMessage *msg = receive();

    // extract statistics and write out to vector file
    simtime_t created = msg->creationTime(),
              sent    = msg->par("sendtime"),
              arrived = msg->arrivalTime();
    queuing_time.record( sent - created );
    transm_time.record( arrived - sent );

#ifdef TRACE_MSG
    ev.printf("sink[%d]: Message received: ",my_address);
    ev.printf("          Name: %s\n",msg->name());
    ev.printf("sink[%d]:   - time between creation and arrival: %lf\n",
              my_address, arrived-created);
#endif

    // message no longer needed
    delete msg;
  }
}
