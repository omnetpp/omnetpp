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
  Copyright (C) 1992-2001 Andras Varga
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
SIM_API int is_started_as_master();

//===========================================================================

/**
 * Base class for the interface module for distributed simulation.
 *
 * This is the abstract base class of the classes supporting
 * distributed simulation in various environments.
 *
 * The class definition for a given environment may be inherited from
 * this class. In the simulation, the global pointer of the network
 * module (simulation.netmodp) is of type cNetMod. The advantage of this
 * base class is that the network interface can be replaced transparently.
 * The drawback is, that beside the general purpose functions, this
 * class has to also declare all environment specific ones, of course
 * all are virtual.
 *
 * The virtual functions will be redefined in the specific network
 * interface.(e.g.:pvmmod.cc)
 *
 */
class SIM_API cNetMod : public cModule
{
    friend class cSimulation;

  protected:
    int segments;
    cMessage *after_modinit_msg;

  public:

    /**
     * Constructor.
     */
    cNetMod();

    /**
     * Destructor.
     */
    virtual ~cNetMod() {}

    // redefined functions

    /**
     * Returns pointer to a string containing the class name, "cNetMod".
     */
    virtual const char *className() const {return "cNetMod";}

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cNetModIFC";}

    // redefined cModule functions

    /**
     * FIXME: redefined cModule functions
     */
    virtual bool isSimple() {return false;}

    /**
     * MISSINGDOC: cNetMod:void scheduleStart(simtime_t)
     */
    virtual void scheduleStart(simtime_t t) {}

    /**
     * MISSINGDOC: cNetMod:void deleteModule()
     */
    virtual void deleteModule() {}

    /**
     * MISSINGDOC: cNetMod:bool callInitialize(int)
     */
    virtual bool callInitialize(int stage);

    /**
     * MISSINGDOC: cNetMod:void callFinish()
     */
    virtual void callFinish();

    // new functions

    /**
     * FIXME: new functions
     */
    int isLocalMachineIn(cArray& m);   // is local host in 'm' list?

    /**
     * MISSINGDOC: cNetMod:cGate*ingate(int)
     */
    virtual cGate *ingate(int g)=0;

    /**
     * MISSINGDOC: cNetMod:cGate*ingate(char*)
     */
    virtual cGate *ingate(const char *s)=0;

    /**
     * MISSINGDOC: cNetMod:int findingate(char*)
     */
    virtual int findingate(const char *s)=0;

    /**
     * MISSINGDOC: cNetMod:cGate*outgate(int)
     */
    virtual cGate *outgate(int g)=0;

    /**
     * MISSINGDOC: cNetMod:cGate*outgate(char*)
     */
    virtual cGate *outgate(const char *s)=0;

    /**
     * MISSINGDOC: cNetMod:int findoutgate(char*)
     */
    virtual int findoutgate(const char *s)=0;


    /**
     * MISSINGDOC: cNetMod:cNetMod&operator=(cNetMod&)
     */
    virtual cNetMod& operator=(cNetMod& other);

    /**
     * MISSINGDOC: cNetMod:char*localhost()
     */
    virtual const char *localhost()=0;

    /**
     * MISSINGDOC: cNetMod:void restart()
     */
    virtual void restart() {}

    /**
     * MISSINGDOC: cNetMod:void sync_after_modinits()
     */
    virtual void sync_after_modinits()=0; // sync before processing first 'real' event

    /**
     * MISSINGDOC: cNetMod:void process_netmsgs()
     */
    virtual void process_netmsgs()=0;

    /**
     * MISSINGDOC: cNetMod:void process_netmsg_blocking()
     */
    virtual void process_netmsg_blocking()=0;

    /**
     * MISSINGDOC: cNetMod:void send_syncpoint(simtime_t,int)
     */
    virtual void send_syncpoint(simtime_t t, int gate) = 0;

    /**
     * MISSINGDOC: cNetMod:void send_cancelsyncpoint(simtime_t,int)
     */
    virtual void send_cancelsyncpoint(simtime_t t, int gate) = 0;

    /**
     * MISSINGDOC: cNetMod:bool block_on_syncpoint(simtime_t)
     */
    virtual bool block_on_syncpoint(simtime_t t) = 0;

    /**
     * MISSINGDOC: cNetMod:int net_addgate(cModule*,int,char)
     */
    virtual int net_addgate(cModule *mod, int gate, char tp) {return 0;}

    // functions to be called from sim.application (TOmnetApp descendant)

    /**
     * FIXME: functions to be called from sim.application (TOmnetApp descendant)
     */
    virtual void putmsg_onconsole(const char *s) = 0;

    /**
     * MISSINGDOC: cNetMod:void puts_onconsole(char*)
     */
    virtual void puts_onconsole(const char *s) = 0;

    /**
     * MISSINGDOC: cNetMod:bool gets_onconsole(char*,char*,int)
     */
    virtual bool gets_onconsole(const char *promptstr, char *buf, int len) = 0;

    /**
     * MISSINGDOC: cNetMod:bool askyesno_onconsole(char*)
     */
    virtual bool askyesno_onconsole(const char *question) = 0;

    // control functions

    /**
     * FIXME: control functions
     */
    virtual short start_segments(cArray& host_list, int ac, char *av[]) = 0;

    /**
     * MISSINGDOC: cNetMod:void setup_connections()
     */
    virtual void setup_connections()=0;

    /**
     * MISSINGDOC: cNetMod:void clear()
     */
    virtual void clear() {gatev.clear();}

    /**
     * MISSINGDOC: cNetMod:void stop_all_segments()
     */
    virtual void stop_all_segments()=0;

    /**
     * MISSINGDOC: cNetMod:void request_stopsimulation()
     */
    virtual void request_stopsimulation()=0;

    /**
     * MISSINGDOC: cNetMod:int receive_runnumber()
     */
    virtual int receive_runnumber() = 0;

    /**
     * MISSINGDOC: cNetMod:void send_runnumber(int)
     */
    virtual void send_runnumber(int run_nr) = 0;
};

#endif


