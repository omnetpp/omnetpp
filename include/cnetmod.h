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
  Copyright (C) 1992-2003 Andras Varga

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
 *
 * @ingroup Internals
 */
class SIM_API cNetMod : public cModule
{
    friend class cSimulation;

  protected:
    int segments;
    cMessage *after_modinit_msg;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cNetMod(const cNetMod& m) : cModule(m)
            {setName(m.name());after_modinit_msg=NULL;operator=(m);}

    /**
     * Constructor.
     */
    cNetMod();

    /**
     * Destructor.
     */
    virtual ~cNetMod() {}

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    virtual cNetMod& operator=(const cNetMod& other);
    //@}

    /** @name Redefined cModule member functions. */
    //@{

    /**
     * Returns false.
     */
    virtual bool isSimple() const {return false;}

    /**
     * Not used, implemented because it could not remain pure virtual.
     * This dummy implementation does nothing.
     */
    virtual void scheduleStart(simtime_t t) {}

    /**
     * Not used, implemented because it could not remain pure virtual.
     * This dummy implementation does nothing.
     */
    virtual void deleteModule() {}

    /**
     * Not used, implemented because it could not remain pure virtual.
     * The implementation was copied from cSimpleModule.
     */
    virtual bool callInitialize(int stage);

    /**
     * Not used, implemented because it could not remain pure virtual.
     * The implementation was copied from cSimpleModule.
     */
    virtual void callFinish();
    //@}

    /** @name Miscellaneous. */
    //@{

    /**
     * Does initialization task that can't be done in the constructor.
     */
    virtual void init()=0;
    //@}

    /** @name Information about the local segment. */
    //@{

    /**
     * Returns the name of the local host.
     */
    virtual const char *localhost()=0;

    /**
     * Returns true if the name of the local machine is in the
     * array passed as argument. The array should contain
     * string-valued cPar objects.
     */
    int isLocalMachineIn(const cArray& m);
    //@}

    /** @name Gates. */
    //@{

    /**
     * FIXME: Adds a gate.
     */
    virtual int net_addgate(cModule *mod, int gate, char tp) {return 0;}

    /**
     * Returns the input gate identified by its index.
     */
    virtual cGate *ingate(int g)=0;

    /**
     * Returns the input gate identified by its name.
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
    //@}

    /** @name Sending output to the console.
     *
     * These functions are called on background (non-console) segments
     * by the simulation application (i.e. the cEnvir implementation),
     * to send output to the console.
     */
    //@{

    /**
     * FIXME:
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
    //@}

    /** @name Setting up and stopping a parallel simulation run. */
    //@{

    /**
     * Called on the controlling segment, starts all other segments.
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
    //@}

    /** @name Executing a parallel simulation run. */
    //@{

    /**
     * MISSINGDOC: cNetMod:void restart()
     */
    virtual void restart() {}

    /**
     * MISSINGDOC: cNetMod:void sync_after_modinits()
     */
    virtual void sync_after_modinits()=0; // sync before processing first 'real' event

    /**
     * Process messages arriving from other segments. This call should block
     * if there's no message available at the time of calling.
     */
    virtual void process_netmsgs()=0;

    /**
     * Process messages arriving from other segments. This call should
     * return immediately if there's no message available at the time
     * of calling.
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
    //@}
};

#endif


