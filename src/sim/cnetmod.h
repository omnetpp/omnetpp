//=========================================================================
//
// CNETMOD.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Zoltan Vass, 1996
//
//   Contents:
//      Declaration of class cNetMod:
//            the interface module for distributed simulation.
//
//   Changes/rewrite:
//      Andras Varga  Oct 14 1996 - Aug 1997
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CNETMOD_H
#define __CNETMOD_H

#include "cmodule.h"

class cSimulation;
class cMessage;


// is_started_as_master:
//   called from envir to determine whether this is a master or slave
int is_started_as_master();

//===========================================================================
//  cNetMod:
//           base class for the interface module for distributed simulation.
//
//      This is the abstract base class of the classes supporting
//      distributed simulation in various environments.
//      The class definition for a given environment may be inherited from
//      this class. In the simulation, the global pointer of the network
//      module (simulation.netmodp) is of type cNetMod. The advantage of this
//      base class is that the network interface can be replaced transparently.
//      The drawback is, that beside the general purpose functions, this
//      class has to also declare all environment specific ones, of course
//      all are virtual.
//      The virtual functions will be redefined in the specific network
//      interface.(e.g.:pvmmod.cc)
//

class cNetMod : public cModule
{
    friend class cSimulation;

  protected:
    int segments;
    cMessage *after_modinit_msg;

  public:
    cNetMod();
    virtual ~cNetMod() {}

    // redefined functions
    virtual const char *className()  {return "cNetMod";}
    virtual const char *inspectorFactoryName() {return "cNetModIFC";}

    // redefined cModule functions
    virtual bool isSimple() {return FALSE;}
    virtual void scheduleStart(simtime_t t) {}
    virtual void deleteModule() {}
    virtual void callInitialize() {initialize();}
    virtual void callFinish() {finish();}

    // new functions
    int isLocalMachineIn(cArray& m);   // is local host in 'm' list?
    virtual cGate *ingate(int g)=0;
    virtual cGate *ingate(const char *s)=0;
    virtual int findingate(const char *s)=0;
    virtual cGate *outgate(int g)=0;
    virtual cGate *outgate(const char *s)=0;
    virtual int findoutgate(const char *s)=0;

    virtual cNetMod& operator=(cNetMod& other);
    virtual const char *localhost()=0;
    virtual void restart() {}
    virtual void sync_after_modinits()=0; // sync before processing first 'real' event
    virtual void process_netmsgs()=0;
    virtual void process_netmsg_blocking()=0;
    virtual void send_syncpoint(simtime_t t, int gate) = 0;
    virtual void send_cancelsyncpoint(simtime_t t, int gate) = 0;
    virtual bool block_on_syncpoint(simtime_t t) = 0;
    virtual int net_addgate(cModule *mod, int gate, char tp) {return 0;}

    // functions to be called from sim.application (TOmnetApp descendant)
    virtual void putmsg_onconsole(const char *s) = 0;
    virtual void puts_onconsole(const char *s) = 0;
    virtual bool gets_onconsole(const char *promptstr, char *buf, int len) = 0;
    virtual bool askyesno_onconsole(const char *question) = 0;

    // control functions
    virtual short start_segments(cArray& host_list, int ac, char *av[]) = 0;
    virtual void setup_connections()=0;
    virtual void clear() {gatev.clear();}
    virtual void stop_all_segments()=0;
    virtual void request_stopsimulation()=0;
    virtual int receive_runnumber() = 0;
    virtual void send_runnumber(int run_nr) = 0;
};

#endif

