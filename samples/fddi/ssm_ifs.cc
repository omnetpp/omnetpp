// FIXME kill all SSM stuff!!!!!!!!!!!!!!!!!!!!!!!!!11
//==========================================================================
//  SSM_IFS.CC  - Simple module definitions of the SSM I/O interfaces for
//                Discrete System Simulation in OMNeT++
//
//  Author: Gabor.Lencse@hit.bme.hu
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1996,97 Gabor Lencse,
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <omnetpp.h>

#include "ssmifdef.h"
#include "fddi_def.h"
#include "ssm_ifs.h"


//Define_Module( SSM_OIF )
Define_Module_Like( SSM_OIF_by_frequency, SSM_OIF )
Define_Module_Like( SSM_OIF_by_time, SSM_OIF )
Define_Module( SSM_IIF )


void SSM_OIF_by_frequency::activity()
  {
#if 0
  //cOutVector packet_length("OIF-packet-length",1);
  long frequency, counter=0; // if counter reaches frequency: send statistics
  simtime_t last_arrival=0; // time of last packet arrivial, must be initialised to 0
  cLongHistogram *length; // the collected distribution of packet length
  cDoubleHistogram *delay; // the collected distribution of inter-arrival time
  // parameters for the histograms:
  unsigned numcells=20; // number of cell in the histograms
  double range_ext_factor=1; // the the observed range is extended by this value
  unsigned num_firstvals=99; // number of samples collected before transform is done

  frequency  = par("frequency");
  simtime_t now;
  length = new cLongHistogram("length",numcells);
  length->setRangeAuto(num_firstvals,range_ext_factor);
  delay = new cDoubleHistogram("inter-arrival time",numcells);
  delay->setRangeAuto(num_firstvals,range_ext_factor);
  for(;;)
    {
    cMessage *msg = receive();
    // int length = msg->length()-idle_bytes;
    //packet_length.record(length);
    now = simTime();
    delay->collect(now-last_arrival);
    last_arrival=now;
    length->collect(msg->length()/*-idle_bytes*/);
    // DO NOT substract the (no of idle symbols)/2
    if ( ++counter == frequency )
      {
// FILE *debug=fopen(fullPath().c_str(),"a");
// length->saveToFile(debug);
// delay->saveToFile(debug);
// fclose(debug);
      counter = 0;
      cMessage *m = new cMessage("SSM Statistics",SSM_STATISTICS);
      m->parList().add( length );
      m->parList().add( delay );
      // snapshot("OIF", m); // DEBUG
      send(m,"out");
      length = new cLongHistogram("length",numcells);
      length->setRangeAuto(num_firstvals,range_ext_factor);
      delay = new cDoubleHistogram("inter-arrival time",numcells);
      delay->setRangeAuto(num_firstvals,range_ext_factor);
      }
    delete msg;
   }
#endif
  }

void SSM_OIF_by_time::activity()
  {
#if 0
  //ev.printf("OIF start %s\n",fullPath().c_str()); // DEBUG
  // Read the control parameters from file and schedule activities like
  // "send" and "delete", send the necessary syncpoints.
  // This code is ment to be run when simTime()==0 otherwise backschedule may
  // be done.
  char filepathname[100];
  strcpy(filepathname,par("OIFControlFile"));
  FILE *f=fopen(filepathname,"r");
  if ( !f )
    {
    ev.printf("OIF Warning: Cannot open input file %s\n",filepathname);
    ev.printf("No Statistics will be collected by %s\n",fullPath().c_str());
    //endSimulation();
    return;
    }
  char line[101];
  fgets(line,101,f); // the first line is a comment with the ID and namestr of the ring
  while ( !feof(f) ) // process all the lines of the file
    {          // one line may contain one command or it may be a comment line
    if ( !fgets(line,101,f) )
      break; // !! This is a hack: EOF is detected too late...
    // "#" or ";" as a beginning character means: this line is a comment
    if ( line[0]=='#' || line[0]==';' )
      continue;
    // if the word "delete" occurs, then isdelelete:=true, otherwise it is set
    // to false and send is assumed
    int isdelete = strstr(line,"delete") != NULL;
    // both "delete" and "send" may be followed by one of the next 3 constructs
        // every <time>
        // at <time>
        // from <time> to <time> step <time>
    // where time is processed by the strToSimtime(char *) function of OMNeT++
    char *kind, simtime[50];
    if ( (kind=strstr(line,"every")) != NULL )
      { // do the requested command at <time>, 2*<time>, 3*<time>, etc.
      sscanf(kind+5,"%[0-9hmsun ]",simtime);
      double every = strToSimtime(simtime);
      cMessage *m;
      if ( isdelete )
        m = new cMessage("Delete",SSM_OIF_DELETE);
      else
        {
        m = new cMessage("Send",SSM_OIF_SEND);
        syncpoint(every,"out");
        }
      m->addPar("repeat") = every;
      m->addPar("finish") = -1.0; // never finish
      scheduleAt(every,m);
      }
    else if ( (kind=strstr(line,"at")) != NULL )
      { // do the requested command at <time>
      sscanf(kind+2,"%[0-9hmsun ]",simtime);
      double at = strToSimtime(simtime);
      cMessage *m;
      if ( isdelete )
        m = new cMessage("Delete",SSM_OIF_DELETE);
      else
        {
        m = new cMessage("Send",SSM_OIF_SEND);
        syncpoint(at,"out");
        }
      m->addPar("repeat") = 0.0;
      scheduleAt(at,m);
      }
    else if ( (kind=strstr(line, "from")) != NULL )
      { // do the requested command at from, from+i*step <= to, i= 1, 2, 3, ...
      sscanf(kind+4,"%[0-9hmsun ]",simtime);
      double from = strToSimtime(simtime);
      sscanf(strstr(line,"to")+2,"%[0-9hmsun ]",simtime);
      double to = strToSimtime(simtime);
      sscanf(strstr(line,"step")+4,"%[0-9hmsun ]",simtime);
      double step = strToSimtime(simtime);
      cMessage *m;
      if ( isdelete )
        m = new cMessage("Delete",SSM_OIF_DELETE);
      else
        {
        m = new cMessage("Send",SSM_OIF_SEND);
        syncpoint(from,"out");
        }
      m->addPar("repeat") = step;
      m->addPar("finish") = to;
      scheduleAt(from,m);
      }
    }
  //cOutVector packet_length("OIF-packet-length",1);
  simtime_t last_arrival=0; // time of last packet arrivial, must be initialised to 0
  cLongHistogram *length; // the collected distribution of packet length
  cDoubleHistogram *delay; // the collected distribution of inter-arrival time
  // parameters for the histograms:
  unsigned numcells=20; // number of cell in the histograms
  double range_ext_factor=1; // the the observed range is extended by this value
  unsigned num_firstvals=99; // number of samples collected before transform is done
  simtime_t now;
  length = new cLongHistogram("length",numcells);
  length->setRangeAuto(num_firstvals,range_ext_factor);
  delay = new cDoubleHistogram("inter-arrival time",numcells);
  delay->setRangeAuto(num_firstvals,range_ext_factor);
  for(;;)
    {
    cMessage *msg = receive();
    // int length = msg->length()-idle_bytes;
    //packet_length.record(length);
    switch ( msg->kind() )
      {
      case FDDI_FRAME:
        {
        now = simTime();
        delay->collect(now-last_arrival);
        last_arrival=now;
        length->collect(msg->length()/*-idle_bytes*/);
        // DO NOT substract the (no of idle symbols)/2
        delete msg;
        break;
        }
      case SSM_OIF_SEND:
        {
// FILE *debug=fopen(fullPath().c_str(),"a");
// length->saveToFile(debug);
// delay->saveToFile(debug);
// fclose(debug);
        cMessage *m = new cMessage("SSM Statistics",SSM_STATISTICS);
        m->parList().add( length->dup() );
        m->parList().add( delay->dup() );
        // snapshot("OIF", m); // DEBUG
        // before sending the statistics, decide if further syncpoint()
        // is needed
        double repeat=msg->par("repeat");
        if ( repeat==0 )
          delete msg; // send was scheduled by the "at" time construct
        else
          { // send was scheduled by the "every" or the "from..." time construct
          double finish=msg->par("finish");
          double next=simTime()+repeat;
          if ( finish<0 || next<=finish )
            {
            syncpoint(next,"out");
            scheduleAt(next,msg);
            }
          else
            delete msg;
          }
        send(m,"out");
        break;
        }
      case SSM_OIF_DELETE:
        {
        delete length;
        length = new cLongHistogram("length",numcells);
        length->setRangeAuto(num_firstvals,range_ext_factor);
        delete delay;
        delay = new cDoubleHistogram("inter-arrival time",numcells);
        delay->setRangeAuto(num_firstvals,range_ext_factor);
        double repeat=msg->par("repeat");
        if ( repeat==0 )
          delete msg;
        else
          {
          double finish=msg->par("finish");
          double next=simTime()+repeat;
          if ( finish<0 || next<=finish )
            scheduleAt(next,msg);
          else
            delete msg;
          }
        break;
        }
      }
    }
#endif
  }

void SSM_IIF::activity()
  {
#if 0
  cLongHistogram *length=0; // the received distribution of packet length
  cDoubleHistogram *delay=0; // the received distribution of inter-arrival time
  cMessage *current_genpack=0; // pointer to the currently scheduled genarator msg

  for(;;)
    {
    cMessage *msg = receive();
    int msgkind = msg->kind();
    switch ( msgkind )
      {
      case SSM_STATISTICS:
        {
        // snapshot("IIF", msg); // DEBUG
        if ( current_genpack ) // => statistcics have already been received earlier
          {
          delete cancelEvent(current_genpack);
          current_genpack = 0; // to show, that there is no current genpack
          delete length;
          delete delay;
          }
        length = (cLongHistogram *) msg->parList().remove("length");
        delay = (cDoubleHistogram *) msg->parList().remove("inter-arrival time");
// FILE *debug=fopen(fullPath().c_str(),"a");
// length->saveToFile(debug);
// delay->saveToFile(debug);
// fclose(debug);
        delete msg;
        if ( delay->samples() ) // histogram is not empty
          {
          current_genpack = new cMessage("GenPack",SSM_IIF_GENPACK);
          scheduleAt(simTime(),current_genpack);
          }
        break;
        }
      case SSM_IIF_GENPACK:
        {
        char msgname[64];
        strcpy(msgname,"FDDI_FRAME from IIF");
        cMessage *m = new cMessage(msgname, FDDI_FRAME);
        m->addPar("gentime") = simTime();
        int pkt_len = (int) length->random();
        m->setLength(pkt_len/*+idle_bytes*/);
        // DO NOT add the (no of idle symbols)/2
        send(m, "out"); // pass down the message for Token Ring MAC
        scheduleAt(simTime()+delay->random(),msg); // to produce the next message
        break;
        }
      default:
        error("SSM_IIF Error: Bad msgkind: %i in %s", msgkind, fullPath().c_str());
      }
    }
#endif
  }
