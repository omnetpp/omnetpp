//=========================================================================
//
// MPIMOD.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Eric Wu, 2001
//
//   Contents:
//      declaration of class cMpiMod, the interface modul for running
//      simulation on Message Passing Interface.
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2001 Eric Wu
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MPIMOD_H
#define __MPIMOD_H

#include <cnetmod.h>
#include "mpidefs.h"
#include "mpipack.h"

//=========================================================================
class cNetGate : public cGate
{
  private:
    int target_Proc;
    int target_Gate;
  public:
    cNetGate(cNetGate& gate);
    cNetGate(const char *name, int tp);
    virtual ~cNetGate()    {}
    virtual const char *className() const {return "cNetGate";}
    virtual cObject *dup()    {return new cNetGate(*this);}
    cNetGate& operator=(cNetGate& gate);

    void settarget(int proc, int gate_n)   {target_Proc=proc;target_Gate=gate_n;}
    int t_proc()   {return target_Proc;}
    int t_gate()   {return target_Gate;}
};

//=========================================================================
struct sSyncPoint {
    simtime_t t;
    int gate;
    sSyncPoint *next;
};

class cMpiMod : public cNetMod
{
  private:
    int mMy_Rank;     // MPI process ID
    int mSize;        // MPI number of processors

    int mArgc;        // MPI number of argumnet
    char** mArgv;     // MPI argument vector

    int mNum_Outgates, mNum_Ingates; // gate counts
    cArray mOutgates; // seperate list for output gates

    cMpiPack* pack;

 private:
    // multicast the MPI pack
    int multicast(int tag, MPI_Comm comm = MPI_COMM_WORLD);
    // pointer to syncpoint list
    sSyncPoint *syncpoints;

  private:
    void net_sendmsg(cMessage *msg,int ongate); // send cMessage over MPI
    void do_process_netmsg(int tag);      // core of process_netmsgs
    void synchronize();                     // waits until all segments call synchronize()

    // syncpoints
    void add_syncpoint(simtime_t t, int gate);
    void del_syncpoint(simtime_t msgtime, int gate);
    void cancel_syncpoint(simtime_t t, int gate);
    simtime_t next_syncpoint();

  public:
    cMpiMod();
    virtual ~cMpiMod();
    virtual const char *className() const {return "cPvmMod";}
    virtual void info(char *buf);
    virtual void callInitialize();
    virtual void callFinish();

    // redefined virtual functions
    virtual void sync_after_modinits(); // sync before processing first 'real' event
    virtual void process_netmsgs();
    virtual void process_netmsg_blocking();
    virtual void send_syncpoint( simtime_t t, int gate);
    virtual void send_cancelsyncpoint( simtime_t t, int gate);
    virtual bool block_on_syncpoint( simtime_t t);
    virtual void arrived(cMessage *msg,int ongate); // sends cMessage over PVM

    // redefined cNetMod virtual functions
    // (to be called from environment classes (TOmnetApp descendants))
    virtual void putmsg_onconsole(const char *s);
    virtual void puts_onconsole(const char *s);
    virtual bool gets_onconsole(const char *promptstr, char *buf, int len);
    virtual bool askyesno_onconsole(const char *question);

    // new functions: handling gates
    cGate *ingate(int g) {return (cNetGate*)gatev[g];}      // find input gate by num.
    cGate *ingate(const char *s);                           // input gate name
    int findingate(const char *s);                          // input gate num. by name
    cGate *outgate(int g) {return (cNetGate*)mOutgates[g];} // output gate by num.
    cGate *outgate(const char *s);                          // output gate by name
    int findoutgate(const char *s);                         // output gate num by name
    virtual void clear();

    // simulation control
    const char *localhost();
                            // the namestr of the local host
    short start_segments(cArray& host_list,int argc,char* argv[]);
                            // starts up all the simulator on each host
    virtual int net_addgate(cModule *mod, int gate, char tp);
                            // creates a new gate in the MPI module

    virtual void send_runnumber(int run_nr);
    virtual int receive_runnumber(); // called from envir's TSlaveApp
    void setup_connections();        // sets up the connection between the MPI modules

    virtual void stop_all_segments();       // halts the simulation immediately
    virtual void request_stopsimulation();  // asks the console to stop simulation
};

#endif

